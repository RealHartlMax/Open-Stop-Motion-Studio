using System;
using System.Threading;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.Core
{
    public sealed class SonyCameraAdapter : VendorCameraAdapterBase
    {
        private readonly CameraDeviceDescriptor? _descriptor;
        private SonyCrSdkSession? _sdkSession;
        private CancellationTokenSource? _liveViewCancellation;
        private Task? _liveViewTask;
        private CameraResolution _lastResolution = new(0, 0);

        public override string Name => "Sony DSLR Adapter";
        public override bool UsesSdkStillCapture => _sdkSession != null;

        public SonyCameraAdapter()
        {
        }

        public SonyCameraAdapter(CameraDeviceDescriptor descriptor)
        {
            _descriptor = descriptor;
        }

        public override bool Connect(int deviceIndex)
        {
            Disconnect();

            if (_descriptor?.ConnectionKind == CameraConnectionKind.SonyCr && OperatingSystem.IsWindows())
            {
                try
                {
                    SonySdkLocation? sdkLocation = new SonySdkDiscovery().FindSdk();
                    if (sdkLocation is not null)
                    {
                        _sdkSession = SonyCrSdkSession.Open(sdkLocation, _descriptor.ConnectionToken);
                        RaiseStatusChanged($"Sony CrSDK verbunden: {_sdkSession.CameraName}");
                    }
                }
                catch (Exception ex)
                {
                    RaiseStatusChanged($"Sony CrSDK-Verbindung fehlgeschlagen: {ex.Message}");
                }
            }

            bool connected = base.Connect(deviceIndex);
            if (connected && _sdkSession != null)
            {
                RaiseStatusChanged("Sony-Kamera verbunden. Live-View läuft über generisches Back-End, CrSDK-Verbindung ist aktiv.");
                return true;
            }

            if (_sdkSession != null)
            {
                IsConnected = true;
                RaiseStatusChanged("Sony CrSDK verbunden. Generischer Live-View-Stream ist nicht verfügbar.");
                return true;
            }

            return connected;
        }

        public override void Disconnect()
        {
            StopLiveView();
            base.Disconnect();
            _sdkSession?.Dispose();
            _sdkSession = null;
            IsConnected = false;
        }

        public override void StartLiveView()
        {
            if (_sdkSession == null)
            {
                base.StartLiveView();
                return;
            }

            if (_liveViewTask != null && !_liveViewTask.IsCompleted)
                return;

            _sdkSession.StartLiveView();
            _liveViewCancellation = new CancellationTokenSource();
            _liveViewTask = Task.Run(() => LiveViewLoop(_liveViewCancellation.Token), _liveViewCancellation.Token);
            RaiseStatusChanged("Sony Live-View über CrSDK gestartet.");
        }

        public override void StopLiveView()
        {
            if (_sdkSession == null)
            {
                base.StopLiveView();
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
                }
                finally
                {
                    _liveViewTask = null;
                    _liveViewCancellation.Dispose();
                    _liveViewCancellation = null;
                }
            }

            _sdkSession.StopLiveView();
        }

        public override void CaptureImage()
        {
            if (_sdkSession != null)
            {
                try
                {
                    string filePath = _sdkSession.TriggerStillCapture();
                    RaiseStatusChanged($"Sony Capture über CrSDK empfangen: {System.IO.Path.GetFileName(filePath)}");
                    RaiseImageCaptured(filePath);
                    return;
                }
                catch (Exception ex)
                {
                    RaiseStatusChanged($"Sony Capture über CrSDK fehlgeschlagen: {ex.Message}");
                }
            }

            RaiseStatusChanged("Sony Capture: Fallback auf generisches Video-Backend.");
            base.CaptureImage();
        }

        public override CameraResolution GetCurrentResolution()
        {
            return _sdkSession?.CurrentResolution ?? base.GetCurrentResolution();
        }

        private void LiveViewLoop(CancellationToken token)
        {
            while (!token.IsCancellationRequested)
            {
                try
                {
                    byte[]? frame = _sdkSession?.TryGetLiveViewJpegFrame();
                    if (frame is { Length: > 0 })
                    {
                        _lastResolution = _sdkSession!.CurrentResolution;
                        RaiseLiveViewFrame(frame);
                    }
                }
                catch (Exception ex)
                {
                    RaiseStatusChanged($"Sony Live-View fehlgeschlagen: {ex.Message}");
                    return;
                }

                Thread.Sleep(33);
            }
        }
    }
}
