using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.Core
{
    public sealed class NikonCameraAdapter : CameraAdapterBase
    {
        private readonly CameraDeviceDescriptor _descriptor;
        private readonly NikonSdkDiscovery _sdkDiscovery = new();
        private readonly DirectShowCameraAdapter _fallbackAdapter = new();
        private readonly object _sdkSync = new();

        private NikonMaidSession? _sdkSession;
        private CancellationTokenSource? _liveViewCancellation;
        private Task? _liveViewTask;
        private CameraResolution _lastResolution = new(0, 0);

        public NikonCameraAdapter(CameraDeviceDescriptor descriptor)
        {
            _descriptor = descriptor;

            _fallbackAdapter.OnLiveViewFrame += frame =>
            {
                _lastResolution = TryGetJpegResolution(frame, out CameraResolution resolution) ? resolution : _lastResolution;
                RaiseLiveViewFrame(frame);
            };
            _fallbackAdapter.OnImageCaptured += RaiseImageCaptured;
            _fallbackAdapter.OnDisconnected += HandleFallbackDisconnected;
            _fallbackAdapter.OnStatusChanged += RaiseStatusChanged;
        }

        public override string Name => "Nikon DSLR Adapter";
        public override bool UsesSdkStillCapture => _sdkSession != null;

        public override bool Connect(int deviceIndex)
        {
            Disconnect();

            if (OperatingSystem.IsWindows())
            {
                try
                {
                    NikonMaidSdkLocation? sdkLocation = _sdkDiscovery.ResolveMaidSdk(_descriptor.ConnectionToken, _descriptor.Name);
                    if (sdkLocation is not null)
                    {
                        _sdkSession = NikonMaidSession.Open(sdkLocation, _descriptor.Name);
                        IsConnected = true;
                        RaiseStatusChanged($"Nikon MAID verbunden: {_sdkSession.SourceName}");
                        return true;
                    }
                }
                catch (Exception ex)
                {
                    RaiseStatusChanged($"Nikon MAID-Verbindung fehlgeschlagen: {ex.Message}");
                }
            }

            if (deviceIndex >= 0 && _fallbackAdapter.Connect(deviceIndex))
            {
                IsConnected = true;
                RaiseStatusChanged("Nikon-Kamera verbunden. Fallback auf generisches Video-Backend.");
                return true;
            }

            return false;
        }

        public override void Disconnect()
        {
            bool hadConnection = IsConnected || _sdkSession != null || _fallbackAdapter.IsConnected;

            StopLiveView();
            _fallbackAdapter.Disconnect();

            lock (_sdkSync)
            {
                _sdkSession?.Dispose();
                _sdkSession = null;
            }

            IsConnected = false;
            if (hadConnection)
                RaiseDisconnected();
        }

        public override void StartLiveView()
        {
            if (_sdkSession == null)
            {
                _fallbackAdapter.StartLiveView();
                return;
            }

            if (_liveViewTask != null && !_liveViewTask.IsCompleted)
                return;

            try
            {
                lock (_sdkSync)
                {
                    _sdkSession?.StartLiveView();
                }

                _liveViewCancellation = new CancellationTokenSource();
                _liveViewTask = Task.Run(() => LiveViewLoop(_liveViewCancellation.Token), _liveViewCancellation.Token);
                RaiseStatusChanged("Nikon Live-View über MAID gestartet.");
            }
            catch (Exception ex)
            {
                RaiseStatusChanged($"Nikon Live-View konnte nicht gestartet werden: {ex.Message}");
            }
        }

        public override void StopLiveView()
        {
            if (_sdkSession == null)
            {
                _fallbackAdapter.StopLiveView();
                return;
            }

            if (_liveViewCancellation != null)
            {
                _liveViewCancellation.Cancel();
                try
                {
                    _liveViewTask?.Wait(1000);
                }
                catch (AggregateException)
                {
                    // ignore cancellation races from shutdown
                }
                finally
                {
                    _liveViewTask = null;
                    _liveViewCancellation.Dispose();
                    _liveViewCancellation = null;
                }
            }

            try
            {
                lock (_sdkSync)
                {
                    _sdkSession?.StopLiveView();
                }
            }
            catch (Exception ex)
            {
                RaiseStatusChanged($"Nikon Live-View-Stopp fehlgeschlagen: {ex.Message}");
            }
        }

        public override void CaptureImage()
        {
            if (_sdkSession == null)
            {
                _fallbackAdapter.CaptureImage();
                return;
            }

            _ = Task.Run(() =>
            {
                try
                {
                    string filePath;
                    lock (_sdkSync)
                    {
                        if (_sdkSession == null)
                            throw new InvalidOperationException("Die Nikon-Sitzung ist nicht mehr aktiv.");

                        filePath = _sdkSession.CaptureImage();
                    }

                    RaiseStatusChanged($"Nikon-Aufnahme empfangen: {System.IO.Path.GetFileName(filePath)}");
                    RaiseImageCaptured(filePath);
                }
                catch (Exception ex)
                {
                    RaiseStatusChanged($"Nikon-Aufnahme fehlgeschlagen: {ex.Message}");
                }
            });
        }

        public override void SetProperty(string property, object value)
        {
            if (_sdkSession == null)
            {
                _fallbackAdapter.SetProperty(property, value);
                return;
            }

            if (string.Equals(property, "FrameWidth", StringComparison.OrdinalIgnoreCase)
                || string.Equals(property, "FrameHeight", StringComparison.OrdinalIgnoreCase))
            {
                RaiseStatusChanged("Nikon MAID-Live-View verwendet die Kamera-Auflösung. Die UI-Auswahl dient hier nur als Referenz.");
                return;
            }

            RaiseStatusChanged($"Nikon MAID-Eigenschaft aktuell nicht gemappt: {property}");
        }

        public override CameraResolution GetCurrentResolution() =>
            _sdkSession == null ? _fallbackAdapter.GetCurrentResolution() : _lastResolution;

        private void LiveViewLoop(CancellationToken token)
        {
            while (!token.IsCancellationRequested)
            {
                try
                {
                    byte[]? frame;
                    lock (_sdkSync)
                    {
                        if (_sdkSession == null)
                            return;

                        frame = _sdkSession.TryGetLiveViewJpegFrame();
                    }

                    if (frame is { Length: > 0 })
                    {
                        if (TryGetJpegResolution(frame, out CameraResolution resolution))
                            _lastResolution = resolution;

                        RaiseLiveViewFrame(frame);
                    }
                }
                catch (Exception ex)
                {
                    RaiseStatusChanged($"Nikon Live-View fehlgeschlagen: {ex.Message}");
                    return;
                }

                Thread.Sleep(80);
            }
        }

        private void HandleFallbackDisconnected()
        {
            IsConnected = false;
            RaiseDisconnected();
        }

        private static bool TryGetJpegResolution(IReadOnlyList<byte> jpegData, out CameraResolution resolution)
        {
            resolution = new CameraResolution(0, 0);

            for (int i = 0; i < jpegData.Count - 9; i++)
            {
                if (jpegData[i] != 0xFF)
                    continue;

                byte marker = jpegData[i + 1];
                if (marker is not (>= 0xC0 and <= 0xC3) and not (>= 0xC5 and <= 0xC7) and not (>= 0xC9 and <= 0xCB) and not (>= 0xCD and <= 0xCF))
                    continue;

                int height = (jpegData[i + 5] << 8) | jpegData[i + 6];
                int width = (jpegData[i + 7] << 8) | jpegData[i + 8];
                resolution = new CameraResolution(width, height);
                return width > 0 && height > 0;
            }

            return false;
        }
    }
}
