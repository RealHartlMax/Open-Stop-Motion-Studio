using System;

namespace OpenStopMotionStudio.Core
{
    public abstract class CameraAdapterBase : ICameraAdapter
    {
        public abstract string Name { get; }
        public bool IsConnected { get; protected set; }
        public virtual bool UsesSdkStillCapture => false;

        public abstract bool Connect(int deviceIndex);
        public abstract void Disconnect();

        public abstract void StartLiveView();
        public abstract void StopLiveView();

        public abstract void CaptureImage();
        public abstract void SetProperty(string property, object value);

        public abstract CameraResolution GetCurrentResolution();

        public event Action<byte[]>? OnLiveViewFrame;
        public event Action<string>? OnImageCaptured;
        public event Action? OnDisconnected;
        public event Action<string>? OnStatusChanged;

        protected void RaiseLiveViewFrame(byte[] frameData) => OnLiveViewFrame?.Invoke(frameData);

        protected void RaiseImageCaptured(string filePath) => OnImageCaptured?.Invoke(filePath);

        protected void RaiseDisconnected() => OnDisconnected?.Invoke();

        protected void RaiseStatusChanged(string message) => OnStatusChanged?.Invoke(message);
    }
}
