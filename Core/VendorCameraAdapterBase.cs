using System;

namespace OpenStopMotionStudio.Core
{
    public abstract class VendorCameraAdapterBase : CameraAdapterBase
    {
        private readonly DirectShowCameraAdapter _innerAdapter = new();

        protected VendorCameraAdapterBase()
        {
            _innerAdapter.OnLiveViewFrame += RaiseLiveViewFrame;
            _innerAdapter.OnImageCaptured += RaiseImageCaptured;
            _innerAdapter.OnDisconnected += () =>
            {
                IsConnected = false;
                RaiseDisconnected();
            };
            _innerAdapter.OnStatusChanged += RaiseStatusChanged;
        }

        public override bool Connect(int deviceIndex)
        {
            if (!_innerAdapter.Connect(deviceIndex))
                return false;

            IsConnected = true;
            RaiseStatusChanged($"[{Name}] Verbunden über generisches Video-Backend.");
            return true;
        }

        public override void Disconnect()
        {
            _innerAdapter.Disconnect();
            IsConnected = false;
        }

        public override void StartLiveView() => _innerAdapter.StartLiveView();

        public override void StopLiveView() => _innerAdapter.StopLiveView();

        public override void CaptureImage() => _innerAdapter.CaptureImage();

        public override void SetProperty(string property, object value)
        {
            if (string.Equals(property, "Exposure", StringComparison.OrdinalIgnoreCase) && value is double exposure)
            {
                _innerAdapter.SetProperty("Exposure", exposure);
                return;
            }

            _innerAdapter.SetProperty(property, value);
        }

        public override CameraResolution GetCurrentResolution() => _innerAdapter.GetCurrentResolution();
    }
}
