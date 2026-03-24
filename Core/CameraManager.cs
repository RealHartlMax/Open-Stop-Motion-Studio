using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using DirectShowLib;

namespace OpenStopMotionStudio.Core
{
    /// <summary>
    /// CameraManager: Verantwortlich für alles rund um die Kamera.
    /// </summary>
    public class CameraManager : ISampleGrabberCB, IDisposable
    {
        public event Action<BitmapSource>? FrameReady;

        public bool IsRunning { get; private set; }
        public bool CanOpenDeviceSettings => _sourceFilter is ISpecifyPropertyPages;

        private IFilterGraph2? _filterGraph;
        private IMediaControl? _mediaControl;
        private ISampleGrabber? _sampleGrabber;
        private ICaptureGraphBuilder2? _captureGraphBuilder;
        private IBaseFilter? _sourceFilter;
        private IBaseFilter? _grabberFilter;
        private IBaseFilter? _nullRenderer;
        private BitmapSource? _currentFrame;
        private readonly object _frameLock = new();
        private int _frameWidth;
        private int _frameHeight;
        private int _frameStride;
        private bool _flipVertical;

        public List<string> GetAvailableDevices()
        {
            var result = new List<string>();

            try
            {
                var devices = DsDevice.GetDevicesOfCat(FilterCategory.VideoInputDevice);
                foreach (var device in devices)
                    result.Add(device.Name ?? "Unbekannte Kamera");
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[CameraManager] GetDevices failed: {ex.Message}");
            }

            return result;
        }

        public bool Start(int deviceIndex)
        {
            Stop();
            ReleaseGraph();

            try
            {
                var devices = DsDevice.GetDevicesOfCat(FilterCategory.VideoInputDevice);
                if (deviceIndex < 0 || deviceIndex >= devices.Length)
                    return false;

                _filterGraph = (IFilterGraph2)new FilterGraph();
                _mediaControl = (IMediaControl)_filterGraph;
                _captureGraphBuilder = (ICaptureGraphBuilder2)new CaptureGraphBuilder2();

                DsError.ThrowExceptionForHR(_captureGraphBuilder.SetFiltergraph(_filterGraph));

                DsError.ThrowExceptionForHR(_filterGraph.AddSourceFilterForMoniker(
                    devices[deviceIndex].Mon,
                    null,
                    devices[deviceIndex].Name,
                    out _sourceFilter!));

                _sampleGrabber = (ISampleGrabber)new SampleGrabber();
                _grabberFilter = (IBaseFilter)_sampleGrabber;
                DsError.ThrowExceptionForHR(_filterGraph.AddFilter(_grabberFilter, "Sample Grabber"));

                var mediaType = new AMMediaType
                {
                    majorType = MediaType.Video,
                    subType = MediaSubType.RGB24,
                    formatType = FormatType.VideoInfo
                };

                try
                {
                    DsError.ThrowExceptionForHR(_sampleGrabber.SetMediaType(mediaType));
                }
                finally
                {
                    DsUtils.FreeAMMediaType(mediaType);
                }

                DsError.ThrowExceptionForHR(_sampleGrabber.SetOneShot(false));
                DsError.ThrowExceptionForHR(_sampleGrabber.SetBufferSamples(false));
                DsError.ThrowExceptionForHR(_sampleGrabber.SetCallback(this, 1));

                _nullRenderer = (IBaseFilter)new NullRenderer();
                DsError.ThrowExceptionForHR(_filterGraph.AddFilter(_nullRenderer, "Null Renderer"));

                DsError.ThrowExceptionForHR(_captureGraphBuilder.RenderStream(
                    PinCategory.Capture,
                    MediaType.Video,
                    _sourceFilter,
                    _grabberFilter,
                    _nullRenderer));

                ReadConnectedMediaType();
                DsError.ThrowExceptionForHR(_mediaControl.Run());

                IsRunning = true;
                return true;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[CameraManager] Start failed: {ex.Message}");
                Stop();
                ReleaseGraph();
                return false;
            }
        }

        private void ReadConnectedMediaType()
        {
            if (_sampleGrabber == null)
                throw new InvalidOperationException("SampleGrabber ist nicht initialisiert.");

            var mediaType = new AMMediaType();

            try
            {
                DsError.ThrowExceptionForHR(_sampleGrabber.GetConnectedMediaType(mediaType));

                if (mediaType.formatPtr == IntPtr.Zero || mediaType.formatType != FormatType.VideoInfo)
                    throw new InvalidOperationException("Unerwartetes Videoformat vom Kameratreiber.");

                var videoInfo = Marshal.PtrToStructure<VideoInfoHeader>(mediaType.formatPtr)
                    ?? throw new InvalidOperationException("Konnte VideoInfoHeader nicht lesen.");
                _frameWidth = videoInfo.BmiHeader.Width;
                _frameHeight = Math.Abs(videoInfo.BmiHeader.Height);
                _flipVertical = videoInfo.BmiHeader.Height > 0;

                int bitsPerPixel = Math.Max(videoInfo.BmiHeader.BitCount, (short)24);
                int calculatedStride = ((_frameWidth * bitsPerPixel + 31) / 32) * 4;
                _frameStride = videoInfo.BmiHeader.ImageSize > 0 && _frameHeight > 0
                    ? videoInfo.BmiHeader.ImageSize / _frameHeight
                    : calculatedStride;
            }
            finally
            {
                DsUtils.FreeAMMediaType(mediaType);
            }
        }

        public int SampleCB(double sampleTime, IMediaSample mediaSample) => 0;

        public int BufferCB(double sampleTime, IntPtr buffer, int bufferLen)
        {
            if (!IsRunning || buffer == IntPtr.Zero || bufferLen <= 0 || _frameWidth <= 0 || _frameHeight <= 0)
                return 0;

            try
            {
                var sourceStride = _frameStride > 0 ? _frameStride : Math.Max(bufferLen / _frameHeight, _frameWidth * 3);
                var sourceBuffer = new byte[bufferLen];
                Marshal.Copy(buffer, sourceBuffer, 0, bufferLen);

                byte[] normalizedBuffer;
                if (_flipVertical)
                {
                    normalizedBuffer = new byte[bufferLen];

                    for (int y = 0; y < _frameHeight; y++)
                    {
                        int sourceOffset = y * sourceStride;
                        int targetOffset = (_frameHeight - 1 - y) * sourceStride;
                        Buffer.BlockCopy(sourceBuffer, sourceOffset, normalizedBuffer, targetOffset, sourceStride);
                    }
                }
                else
                {
                    normalizedBuffer = sourceBuffer;
                }

                var frame = BitmapSource.Create(
                    _frameWidth,
                    _frameHeight,
                    96,
                    96,
                    PixelFormats.Bgr24,
                    null,
                    normalizedBuffer,
                    sourceStride);
                frame.Freeze();

                lock (_frameLock)
                    _currentFrame = frame;

                FrameReady?.Invoke(frame);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[CameraManager] Buffer callback failed: {ex.Message}");
            }

            return 0;
        }

        public void Stop()
        {
            IsRunning = false;

            try
            {
                _mediaControl?.Stop();
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[CameraManager] Stop failed: {ex.Message}");
            }

            lock (_frameLock)
                _currentFrame = null;
        }

        public BitmapSource? GetCurrentFrame()
        {
            lock (_frameLock)
                return _currentFrame;
        }

        public bool OpenDeviceSettings(IntPtr ownerWindowHandle)
        {
            if (_sourceFilter is not ISpecifyPropertyPages propertyPages)
                return false;

            DsCAUUID pageCollection = new();

            try
            {
                DsError.ThrowExceptionForHR(propertyPages.GetPages(out pageCollection));
                Guid[] pageIds = pageCollection.ToGuidArray();
                if (pageIds.Length == 0)
                    return false;

                object cameraSource = _sourceFilter;
                int hr = OleCreatePropertyFrame(
                    ownerWindowHandle,
                    0,
                    0,
                    "Kameraeinstellungen",
                    1,
                    ref cameraSource,
                    pageIds.Length,
                    pageIds,
                    0,
                    0,
                    IntPtr.Zero);

                return hr >= 0;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[CameraManager] OpenDeviceSettings failed: {ex.Message}");
                return false;
            }
            finally
            {
                if (pageCollection.pElems != IntPtr.Zero)
                    Marshal.FreeCoTaskMem(pageCollection.pElems);
            }
        }

        public void Dispose()
        {
            Stop();
            ReleaseGraph();
        }

        private void ReleaseGraph()
        {
            ReleaseComObject(ref _nullRenderer);
            ReleaseComObject(ref _grabberFilter);
            ReleaseComObject(ref _sourceFilter);
            ReleaseComObject(ref _captureGraphBuilder);
            ReleaseComObject(ref _sampleGrabber);
            ReleaseComObject(ref _mediaControl);
            ReleaseComObject(ref _filterGraph);

            _frameWidth = 0;
            _frameHeight = 0;
            _frameStride = 0;
            _flipVertical = false;
        }

        private static void ReleaseComObject<T>(ref T? comObject) where T : class
        {
            if (comObject == null)
                return;

            try
            {
                if (Marshal.IsComObject(comObject))
                    Marshal.ReleaseComObject(comObject);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[CameraManager] ReleaseComObject failed: {ex.Message}");
            }
            finally
            {
                comObject = null;
            }
        }

        [DllImport("oleaut32.dll", CharSet = CharSet.Unicode)]
        private static extern int OleCreatePropertyFrame(
            IntPtr hwndOwner,
            int x,
            int y,
            string lpszCaption,
            int cObjects,
            [MarshalAs(UnmanagedType.Interface)] ref object ppUnk,
            int cPages,
            [MarshalAs(UnmanagedType.LPArray)] Guid[] pPageClsID,
            int lcid,
            int dwReserved,
            IntPtr pvReserved);
    }
}
