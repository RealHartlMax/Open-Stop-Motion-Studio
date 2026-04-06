using System;

namespace OpenStopMotionStudio.Core
{
    public interface ICameraAdapter
    {
        string Name { get; }
        bool IsConnected { get; }
        bool UsesSdkStillCapture { get; }

        bool Connect(int deviceIndex);
        void Disconnect();

        event Action<byte[]>? OnLiveViewFrame;
        event Action<string>? OnImageCaptured;
        event Action? OnDisconnected;
        event Action<string>? OnStatusChanged;

        void StartLiveView();
        void StopLiveView();

        void CaptureImage();
        void SetProperty(string property, object value);

        CameraResolution GetCurrentResolution();
    }
}
