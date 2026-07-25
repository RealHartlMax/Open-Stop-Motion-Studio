using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Emgu.CV;
using Emgu.CV.CvEnum;

namespace OpenStopMotionStudio.Core
{
    public sealed class DirectShowCameraAdapter : CameraAdapterBase
    {
        private VideoCapture? _videoCapture;
        private CancellationTokenSource? _cancellationTokenSource;
        private Task? _captureTask;
        private byte[]? _lastFrameData;
        private int _requestedWidth;
        private int _requestedHeight;

        public override string Name => "DirectShow Adapter";

        public override bool Connect(int deviceIndex)
        {
            if (_videoCapture != null || IsConnected || _captureTask != null || _cancellationTokenSource != null)
                Disconnect();

            VideoCapture.API[] backends = OperatingSystem.IsWindows()
                ? new[] { VideoCapture.API.Msmf, VideoCapture.API.Any }
                : new[] { VideoCapture.API.Any };

            int maxAttemptsPerBackend = 2;
            var failures = new List<string>();

            foreach (var backend in backends)
            {
                for (int attempt = 1; attempt <= maxAttemptsPerBackend; attempt++)
                {
                    try
                    {
                        _videoCapture = new VideoCapture(deviceIndex, backend);
                        if (_videoCapture.IsOpened)
                        {
                            if (_requestedWidth > 0 && _requestedHeight > 0)
                            {
                                _videoCapture.Set(CapProp.FrameWidth, _requestedWidth);
                                _videoCapture.Set(CapProp.FrameHeight, _requestedHeight);
                            }

                            IsConnected = true;
                            RaiseStatusChanged($"Connected to camera index {deviceIndex} using {backend}.");
                            return true;
                        }

                        failures.Add($"Connect failed using {backend} (attempt {attempt}): device did not open.");
                    }
                    catch (Exception ex)
                    {
                        failures.Add($"Connect failed using {backend} (attempt {attempt}): {ex.GetBaseException().Message}");
                    }
                    finally
                    {
                        if (_videoCapture != null && !_videoCapture.IsOpened)
                        {
                            _videoCapture.Dispose();
                            _videoCapture = null;
                        }
                    }

                    if (attempt < maxAttemptsPerBackend)
                        Thread.Sleep(150);
                }
            }

            if (failures.Count > 0)
            {
                string combinedFailure = string.Join(Environment.NewLine, failures.Distinct());
                RaiseStatusChanged(combinedFailure);
            }
            else
            {
                RaiseStatusChanged($"Connect failed for camera index {deviceIndex}: no backend succeeded.");
            }

            Disconnect();
            return false;
        }

        public override void Disconnect()
        {
            bool hadActiveResources = _videoCapture != null
                || IsConnected
                || _captureTask != null
                || _cancellationTokenSource != null;

            StopLiveView();

            try
            {
                _videoCapture?.Dispose();
            }
            catch (Exception ex)
            {
                RaiseStatusChanged($"Disconnect error: {ex.Message}");
            }
            finally
            {
                _videoCapture = null;
                IsConnected = false;

                if (hadActiveResources)
                    RaiseDisconnected();
            }
        }

        public override void StartLiveView()
        {
            if (!IsConnected || _videoCapture == null)
                return;

            if (_captureTask != null && !_captureTask.IsCompleted)
                return;

            _cancellationTokenSource = new CancellationTokenSource();
            _captureTask = Task.Run(() => CaptureLoop(_cancellationTokenSource.Token), _cancellationTokenSource.Token);
        }

        public override void StopLiveView()
        {
            if (_cancellationTokenSource == null)
                return;

            try
            {
                _cancellationTokenSource.Cancel();
                if (_captureTask != null && !_captureTask.Wait(1000))
                {
                    RaiseStatusChanged("Live view stop timeout.");
                }
            }
            catch (Exception ex)
            {
                RaiseStatusChanged($"StopLiveView failed: {ex.Message}");
            }
            finally
            {
                _captureTask = null;
                _cancellationTokenSource.Dispose();
                _cancellationTokenSource = null;
            }
        }

        public override void CaptureImage()
        {
            if (_lastFrameData == null)
            {
                RaiseStatusChanged("Kein Live-Bild für Capture verfügbar.");
                return;
            }

            try
            {
                string outputFolder = Path.Combine(Path.GetTempPath(), "OpenStopMotionStudio", "CaptureTemp");
                Directory.CreateDirectory(outputFolder);

                string fileName = $"capture_{DateTime.Now:yyyyMMdd_HHmmssfff}.jpg";
                string filePath = Path.Combine(outputFolder, fileName);
                File.WriteAllBytes(filePath, _lastFrameData);
                RaiseImageCaptured(filePath);
            }
            catch (Exception ex)
            {
                RaiseStatusChanged($"CaptureImage failed: {ex.Message}");
            }
        }

        public override void SetProperty(string property, object value)
        {
            if (_videoCapture == null)
                return;

            if (string.Equals(property, "FrameWidth", StringComparison.OrdinalIgnoreCase) && value is int width)
            {
                _requestedWidth = width;
                _videoCapture.Set(CapProp.FrameWidth, width);
            }
            else if (string.Equals(property, "FrameHeight", StringComparison.OrdinalIgnoreCase) && value is int height)
            {
                _requestedHeight = height;
                _videoCapture.Set(CapProp.FrameHeight, height);
            }
            else if (string.Equals(property, "Exposure", StringComparison.OrdinalIgnoreCase) && value is double exposure)
            {
                _videoCapture.Set(CapProp.Exposure, exposure);
            }
            else
            {
                RaiseStatusChanged($"Unknown property: {property}");
            }
        }

        public override CameraResolution GetCurrentResolution()
        {
            if (_videoCapture == null || !_videoCapture.IsOpened)
                return new CameraResolution(0, 0);

            int width = (int)_videoCapture.Get(CapProp.FrameWidth);
            int height = (int)_videoCapture.Get(CapProp.FrameHeight);
            return new CameraResolution(width, height);
        }

        public static List<string> GetAvailableDevices()
        {
            return CameraDeviceEnumerator.EnumerateDeviceDescriptors()
                .ConvertAll(descriptor => descriptor.DisplayName);
        }

        private void CaptureLoop(CancellationToken token)
        {
            using var frameMat = new Mat();
            using var bgraMat = new Mat();

            try
            {
                while (!token.IsCancellationRequested && _videoCapture != null && _videoCapture.IsOpened)
                {
                    if (!_videoCapture.Read(frameMat) || frameMat.IsEmpty)
                        continue;

                    CvInvoke.CvtColor(frameMat, bgraMat, ColorConversion.Bgr2Bgra);

                    using var buffer = new Emgu.CV.Util.VectorOfByte();
                    CvInvoke.Imencode(".jpg", bgraMat, buffer);
                    _lastFrameData = buffer.ToArray();
                    RaiseLiveViewFrame(_lastFrameData);

                    Thread.Sleep(16);
                }
            }
            catch (Exception ex)
            {
                RaiseStatusChanged($"Capture loop failed: {ex.Message}");
            }
            finally
            {
                IsConnected = false;
            }
        }
    }
}
