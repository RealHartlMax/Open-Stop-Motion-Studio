using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.Core
{
    public sealed class CanonCameraAdapter : CameraAdapterBase
    {
        private readonly CameraDeviceDescriptor _descriptor;
        private readonly DirectShowCameraAdapter _fallbackAdapter = new();

        private CanonCameraSession? _sdkSession;
        private CancellationTokenSource? _liveViewCancellation;
        private Task? _liveViewTask;
        private CameraResolution _lastResolution = new(0, 0);
        private CanonEdsdkRuntime.CanonObjectEventHandler? _objectEventHandler;
        private CanonEdsdkRuntime.CanonPropertyEventHandler? _propertyEventHandler;
        private CanonEdsdkRuntime.CanonStateEventHandler? _stateEventHandler;
        private readonly string _captureTempFolder = Path.Combine(Path.GetTempPath(), "OpenStopMotionStudio", "CanonCapture");

        public CanonCameraAdapter(CameraDeviceDescriptor descriptor)
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

        public override string Name => "Canon DSLR Adapter";
        public override bool UsesSdkStillCapture => _sdkSession != null;

        public override bool Connect(int deviceIndex)
        {
            Disconnect();

            if (_descriptor.ConnectionKind == CameraConnectionKind.CanonEdsdk && OperatingSystem.IsWindows())
            {
                try
                {
                    var sdkLocation = new CanonSdkDiscovery().FindSdk()
                        ?? throw new InvalidOperationException("Kein lokales Canon EDSDK gefunden.");

                    _sdkSession = CanonEdsdkRuntime.OpenSession(sdkLocation, _descriptor.ConnectionToken);
                    RegisterCanonEventHandlers(_sdkSession.CameraRef);
                    IsConnected = true;
                    RaiseStatusChanged($"Canon EDSDK verbunden: {_descriptor.Name}");
                    return true;
                }
                catch (Exception ex)
                {
                    RaiseStatusChanged($"Canon EDSDK-Verbindung fehlgeschlagen: {ex.Message}");
                }
            }

            if (_fallbackAdapter.Connect(deviceIndex))
            {
                IsConnected = true;
                RaiseStatusChanged("Canon-Kamera verbunden. Fallback auf generisches Video-Backend.");
                return true;
            }

            return false;
        }

        public override void Disconnect()
        {
            bool hadConnection = IsConnected || _sdkSession != null || _fallbackAdapter.IsConnected;

            StopLiveView();
            _fallbackAdapter.Disconnect();

            _sdkSession?.Dispose();
            _sdkSession = null;
            _objectEventHandler = null;
            _propertyEventHandler = null;
            _stateEventHandler = null;
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

            CanonEdsdkRuntime.StartEvf(_sdkSession.CameraRef);

            _liveViewCancellation = new CancellationTokenSource();
            _liveViewTask = Task.Run(() => LiveViewLoop(_liveViewCancellation.Token), _liveViewCancellation.Token);
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

            CanonEdsdkRuntime.StopEvf(_sdkSession.CameraRef);
        }

        public override void CaptureImage()
        {
            if (_sdkSession == null)
            {
                _fallbackAdapter.CaptureImage();
                return;
            }

            try
            {
                CanonEdsdkRuntime.TriggerStillCapture(_sdkSession.CameraRef);
                RaiseStatusChanged("Canon-Ausloeser ueber EDSDK betaetigt.");
            }
            catch (Exception ex)
            {
                RaiseStatusChanged($"Canon-Ausloesen fehlgeschlagen: {ex.Message}");
            }
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
                RaiseStatusChanged("Canon EDSDK-Live-View verwendet die Kamera-Aufloesung. Die UI-Auswahl dient hier nur als Referenz.");
                return;
            }

            RaiseStatusChanged($"Canon EDSDK-Eigenschaft aktuell nicht gemappt: {property}");
        }

        public override CameraResolution GetCurrentResolution() =>
            _sdkSession == null ? _fallbackAdapter.GetCurrentResolution() : _lastResolution;

        private void LiveViewLoop(CancellationToken token)
        {
            while (!token.IsCancellationRequested)
            {
                try
                {
                    if (_sdkSession == null)
                        return;

                    CanonEdsdkRuntime.PumpEvents();
                    byte[]? frame = CanonEdsdkRuntime.TryDownloadEvfFrame(_sdkSession.CameraRef);
                    if (frame != null)
                    {
                        if (TryGetJpegResolution(frame, out CameraResolution resolution))
                            _lastResolution = resolution;

                        RaiseLiveViewFrame(frame);
                    }
                }
                catch (Exception ex)
                {
                    RaiseStatusChanged($"Canon Live-View fehlgeschlagen: {ex.Message}");
                    return;
                }

                Thread.Sleep(60);
            }
        }

        private void RegisterCanonEventHandlers(IntPtr cameraRef)
        {
            _objectEventHandler = HandleObjectEvent;
            _propertyEventHandler = HandlePropertyEvent;
            _stateEventHandler = HandleStateEvent;

            CanonEdsdkRuntime.RegisterHandlers(cameraRef, _objectEventHandler, _propertyEventHandler, _stateEventHandler);
        }

        private uint HandleObjectEvent(uint inEvent, IntPtr inRef, IntPtr inContext)
        {
            try
            {
                if (inRef != IntPtr.Zero && CanonEdsdkRuntime.IsTransferRequestEvent(inEvent))
                {
                    string downloadedPath = CanonEdsdkRuntime.DownloadDirectoryItem(inRef, _captureTempFolder);
                    RaiseStatusChanged($"Canon-Still uebertragen: {Path.GetFileName(downloadedPath)}");
                    RaiseImageCaptured(downloadedPath);
                }
            }
            catch (Exception ex)
            {
                RaiseStatusChanged($"Canon-Stilltransfer fehlgeschlagen: {ex.Message}");
            }
            finally
            {
                CanonEdsdkRuntime.ReleaseRef(inRef);
            }

            return 0;
        }

        private uint HandlePropertyEvent(uint inEvent, uint inPropertyId, uint inParam, IntPtr inContext) => 0;

        private uint HandleStateEvent(uint inEvent, uint inEventData, IntPtr inContext) => 0;

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
