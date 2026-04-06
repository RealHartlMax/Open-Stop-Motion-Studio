using System;
using System.Collections.Generic;
using System.IO;
using Avalonia.Media.Imaging;

namespace OpenStopMotionStudio.Core
{
    /// <summary>
    /// CameraManager: Verwaltet die Verbindung zu einem Kamera-Adapter und stellt
    /// die Live-View- sowie Capture-Schnittstelle bereit.
    /// </summary>
    public class CameraManager : IDisposable
    {
        public static CameraManager Instance { get; } = new CameraManager();

        public event Action<Bitmap>? FrameReady;
        public event Action<string>? ImageCaptured;
        public event Action<string>? StatusChanged;

        public bool IsRunning { get; private set; }
        public string? LastStatusMessage { get; private set; }
        public CameraResolution? RequestedResolution { get; private set; }
        public CameraConnectionKind? CurrentConnectionKind { get; private set; }
        public bool UsesHardwareStillCapture => _adapter?.UsesSdkStillCapture == true;

        private ICameraAdapter? _adapter;
        private readonly List<CameraDeviceDescriptor> _deviceDescriptors = new();
        private Bitmap? _currentFrame;
        private bool _isStopping;

        private CameraManager()
        {
            // Initialization is now handled by DeviceEnumerationTask
        }

        public void RefreshDeviceList()
        {
            _deviceDescriptors.Clear();
            _deviceDescriptors.AddRange(CameraAdapterFactory.EnumerateDevices());
            LastStatusMessage = null;
        }

        public List<CameraDeviceDescriptor> GetAvailableDevices() => new(_deviceDescriptors);

        public bool Start(int selectedDeviceIndex)
        {
            Stop();
            LastStatusMessage = null;

            if (selectedDeviceIndex < 0 || selectedDeviceIndex >= _deviceDescriptors.Count)
                return false;

            var device = _deviceDescriptors[selectedDeviceIndex];
            CurrentConnectionKind = device.ConnectionKind;
            _adapter = CameraAdapterFactory.CreateAdapter(device);
            AttachAdapterEvents(_adapter);

            if (!_adapter.Connect(device.Index))
            {
                DetachAdapterEvents();
                _adapter = null;
                CurrentConnectionKind = null;
                return false;
            }

            if (RequestedResolution != null)
            {
                _adapter.SetProperty("FrameWidth", RequestedResolution.Width);
                _adapter.SetProperty("FrameHeight", RequestedResolution.Height);
            }

            _adapter.StartLiveView();
            IsRunning = true;
            StatusChanged?.Invoke($"{device.AdapterName} gestartet: {device.Name}");
            return true;
        }

        public void Stop()
        {
            _isStopping = true;

            try
            {
                if (_adapter != null && _adapter.IsConnected)
                {
                    _adapter.StopLiveView();
                    _adapter.Disconnect();
                }

                DetachAdapterEvents();
                _adapter = null;
                IsRunning = false;
                CurrentConnectionKind = null;
            }
            finally
            {
                _isStopping = false;
            }
        }

        public Bitmap? GetCurrentFrame() => _currentFrame;

        public CameraResolution GetCurrentResolution()
        {
            return _adapter?.GetCurrentResolution() ?? new CameraResolution(0, 0);
        }

        public void SetRequestedResolution(int width, int height)
        {
            RequestedResolution = new CameraResolution(width, height);
            _adapter?.SetProperty("FrameWidth", width);
            _adapter?.SetProperty("FrameHeight", height);
        }

        public bool TriggerHardwareCapture()
        {
            if (_adapter == null || !IsRunning)
                return false;

            _adapter.CaptureImage();
            return true;
        }

        public List<CameraResolution> GetSupportedResolutions(int deviceIndex)
        {
            return new List<CameraResolution>
            {
                new CameraResolution(320, 240),     // QVGA
                new CameraResolution(640, 360),     // nHD
                new CameraResolution(640, 480),     // VGA
                new CameraResolution(800, 600),     // SVGA
                new CameraResolution(1024, 576),    // WSVGA
                new CameraResolution(1024, 768),    // XGA
                new CameraResolution(1280, 720),    // HD
                new CameraResolution(1280, 960),    // SXGA
                new CameraResolution(1440, 810),    // HD+
                new CameraResolution(1600, 900),    // HD+
                new CameraResolution(1920, 1080),   // FHD
                new CameraResolution(2560, 1440),   // QHD
            };
        }

        private void Adapter_OnLiveViewFrame(byte[] imageData)
        {
            try
            {
                using var stream = new MemoryStream(imageData);
                var bitmap = new Bitmap(stream);
                _currentFrame = bitmap;
                FrameReady?.Invoke(bitmap);
            }
            catch (Exception ex)
            {
                StatusChanged?.Invoke($"Decoding live view failed: {ex.Message}");
            }
        }

        private void Adapter_OnImageCaptured(string filePath)
        {
            ImageCaptured?.Invoke(filePath);
        }

        private void Adapter_OnStatusChanged(string message)
        {
            LastStatusMessage = message;
            StatusChanged?.Invoke(message);
        }

        private void Adapter_OnDisconnected()
        {
            if (_isStopping)
                return;

            LastStatusMessage = "Kameraverbindung getrennt.";
            Stop();
            StatusChanged?.Invoke("Kameraverbindung getrennt.");
        }

        private void AttachAdapterEvents(ICameraAdapter adapter)
        {
            adapter.OnLiveViewFrame += Adapter_OnLiveViewFrame;
            adapter.OnImageCaptured += Adapter_OnImageCaptured;
            adapter.OnStatusChanged += Adapter_OnStatusChanged;
            adapter.OnDisconnected += Adapter_OnDisconnected;
        }

        private void DetachAdapterEvents()
        {
            if (_adapter == null)
                return;

            _adapter.OnLiveViewFrame -= Adapter_OnLiveViewFrame;
            _adapter.OnImageCaptured -= Adapter_OnImageCaptured;
            _adapter.OnStatusChanged -= Adapter_OnStatusChanged;
            _adapter.OnDisconnected -= Adapter_OnDisconnected;
        }

        public void Dispose()
        {
            Stop();
            GC.SuppressFinalize(this);
        }
    }
}
