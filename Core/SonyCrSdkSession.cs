using System;

namespace OpenStopMotionStudio.Core
{
    internal sealed class SonyCrSdkSession : IDisposable
    {
        private readonly SonyCrSdkRuntime.SonyConnection _connection;
        private bool _disposed;

        private SonyCrSdkSession(SonyCrSdkRuntime.SonyConnection connection)
        {
            _connection = connection;
        }

        public string CameraName => _connection.DeviceName;
        public CameraResolution CurrentResolution => new(_connection.LastLiveViewWidth, _connection.LastLiveViewHeight);

        public static SonyCrSdkSession Open(SonySdkLocation sdkLocation, string? preferredId)
        {
            SonyCrSdkRuntime.SonyConnection connection = SonyCrSdkRuntime.ConnectCamera(sdkLocation, preferredId);
            return new SonyCrSdkSession(connection);
        }

        public string TriggerStillCapture()
        {
            ObjectDisposedException.ThrowIf(_disposed, this);
            return _connection.TriggerStillCapture();
        }

        public void StartLiveView()
        {
            ObjectDisposedException.ThrowIf(_disposed, this);
            _connection.StartLiveView();
        }

        public void StopLiveView()
        {
            if (_disposed)
                return;

            _connection.StopLiveView();
        }

        public byte[]? TryGetLiveViewJpegFrame()
        {
            ObjectDisposedException.ThrowIf(_disposed, this);
            return _connection.TryGetLiveViewJpegFrame();
        }

        public void Dispose()
        {
            if (_disposed)
                return;

            _disposed = true;
            _connection.Dispose();
        }
    }
}