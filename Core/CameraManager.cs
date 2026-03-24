using System;
using System.Collections.Generic;
using System.Drawing;
using System.Threading;
using System.Windows.Media.Imaging;
using DirectShowLib;

namespace OpenStopMotionStudio.Core
{
    /// <summary>
    /// CameraManager: Verantwortlich für alles rund um die Kamera.
    ///
    /// Architektur-Prinzip: Dieser Manager ist der einzige Ort in der
    /// gesamten Anwendung, der direkt mit DirectShow interagiert.
    /// Das MainWindow weiß nichts über DirectShow – es bekommt nur
    /// fertige BitmapSource-Objekte geliefert. Dieses Separation-of-Concerns
    /// Prinzip macht es später einfach, DirectShow gegen ein DSLR-SDK
    /// oder Media Foundation auszutauschen.
    /// </summary>
    public class CameraManager : IDisposable
    {
        // ── Öffentliches Event: MainWindow abonniert dies ───────────────────
        /// <summary>
        /// Wird für jeden neuen Frame aufgerufen (~30fps).
        /// Das Event liefert ein WPF-kompatibles BitmapSource-Objekt.
        /// </summary>
        public event Action<BitmapSource>? FrameReady;

        // ── Zustand ─────────────────────────────────────────────────────────
        public bool IsRunning { get; private set; }

        // ── DirectShow Internals ─────────────────────────────────────────────
        // DirectShow läuft in einem eigenen Thread, um den UI-Thread nicht zu blockieren
        private IFilterGraph2?    _filterGraph;
        private IMediaControl?    _mediaControl;
        private ISampleGrabber?   _sampleGrabber;
        private Thread?           _captureThread;
        private bool              _stopRequested;
        private BitmapSource?     _currentFrame;
        private readonly object   _frameLock = new();

        // ════════════════════════════════════════════════════════════════════
        //  GERÄTEERKENNUNG
        // ════════════════════════════════════════════════════════════════════

        /// <summary>
        /// Scannt alle verfügbaren DirectShow Video Capture Devices.
        /// Diese Methode findet USB-Webcams, HDMI-Capture-Karten und
        /// jede Kamera, die sich als DirectShow-Device registriert.
        ///
        /// DSLR-Kameras (Canon EOS, Sony Alpha) werden hier NICHT gefunden –
        /// dafür wird in Phase 2 das jeweilige Hersteller-SDK eingebunden.
        /// </summary>
        public List<string> GetAvailableDevices()
        {
            var result = new List<string>();

            try
            {
                // DirectShow: System nach Video-Capture-Devices fragen
                var devices = DsDevice.GetDevicesOfCat(FilterCategory.VideoInputDevice);
                foreach (var device in devices)
                    result.Add(device.Name ?? "Unbekannte Kamera");
            }
            catch (Exception ex)
            {
                // Kein DirectShow verfügbar oder keine Kamera erkannt
                System.Diagnostics.Debug.WriteLine($"[CameraManager] GetDevices failed: {ex.Message}");
            }

            return result;
        }

        // ════════════════════════════════════════════════════════════════════
        //  KAMERA STARTEN / STOPPEN
        // ════════════════════════════════════════════════════════════════════

        /// <summary>
        /// Startet die Kamera mit dem angegebenen Geräte-Index.
        ///
        /// Intern wird ein DirectShow FilterGraph aufgebaut:
        ///   Source Filter (Kamera) → SampleGrabber Filter → Null Renderer
        /// Der SampleGrabber "schneidet" jeden Frame heraus, ohne den
        /// Datenstrom zu unterbrechen – so bekommen wir den Live-Feed.
        /// </summary>
        public bool Start(int deviceIndex)
        {
            if (IsRunning) Stop();

            try
            {
                var devices = DsDevice.GetDevicesOfCat(FilterCategory.VideoInputDevice);
                if (deviceIndex >= devices.Length) return false;

                // DirectShow FilterGraph aufbauen
                _filterGraph  = (IFilterGraph2)new FilterGraph();
                _mediaControl = (IMediaControl)_filterGraph;

                // 1. Source Filter: die gewählte Kamera
                IBaseFilter sourceFilter;
                _filterGraph.AddSourceFilterForMoniker(
                    devices[deviceIndex].Mon, null,
                    devices[deviceIndex].Name, out sourceFilter);

                // 2. SampleGrabber: Frame-Extraktion
                _sampleGrabber = (ISampleGrabber)new SampleGrabber();
                var grabberFilter = (IBaseFilter)_sampleGrabber;
                _filterGraph.AddFilter(grabberFilter, "Sample Grabber");

                // RGB24 Mediatype setzen: liefert einfach zu verarbeitende Bitmaps
                var mediaType = new AMMediaType
                {
                    majorType  = MediaType.Video,
                    subType    = MediaSubType.RGB24,
                    formatType = FormatType.VideoInfo
                };
                _sampleGrabber.SetMediaType(mediaType);
                _sampleGrabber.SetBufferSamples(true);

                // 3. Null Renderer: DirectShow braucht ein Sink-Filter
                IBaseFilter nullRenderer = (IBaseFilter)new NullRenderer();
                _filterGraph.AddFilter(nullRenderer, "Null Renderer");

                // Pins verbinden: Source → Grabber → NullRenderer
                var captureGraphBuilder = (ICaptureGraphBuilder2)new CaptureGraphBuilder2();
                captureGraphBuilder.SetFiltergraph(_filterGraph);
                captureGraphBuilder.RenderStream(PinCategory.Capture, MediaType.Video,
                    sourceFilter, grabberFilter, nullRenderer);

                // MediaControl startet den Datenstrom
                _mediaControl.Run();
                IsRunning     = true;
                _stopRequested = false;

                // Separater Thread: Frame-Polling ohne UI-Thread zu blockieren
                _captureThread = new Thread(FramePollingLoop)
                {
                    IsBackground = true,
                    Name = "CameraFramePoller"
                };
                _captureThread.Start();

                return true;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[CameraManager] Start failed: {ex.Message}");
                return false;
            }
        }

        /// <summary>
        /// Polling-Schleife im Hintergrund-Thread: zieht kontinuierlich
        /// Frames vom SampleGrabber und konvertiert sie in BitmapSource.
        /// ~30fps entspricht einem Polling-Intervall von 33ms.
        /// </summary>
        private void FramePollingLoop()
        {
            while (!_stopRequested && _sampleGrabber != null)
            {
                try
                {
                    var frame = GrabCurrentFrame();
                    if (frame != null)
                    {
                        lock (_frameLock)
                            _currentFrame = frame;

                        // Event feuern → MainWindow rendert das Bild
                        FrameReady?.Invoke(frame);
                    }
                }
                catch (Exception ex)
                {
                    System.Diagnostics.Debug.WriteLine($"[CameraManager] Frame grab error: {ex.Message}");
                }

                Thread.Sleep(33); // ~30fps
            }
        }

        /// <summary>
        /// Holt einen einzelnen Frame vom SampleGrabber, konvertiert ihn
        /// von raw RGB24-Bytes in ein WPF-kompatibles BitmapSource.
        /// </summary>
        private BitmapSource? GrabCurrentFrame()
        {
            if (_sampleGrabber == null) return null;

            // Schritt 1: Buffer-Größe ermitteln
            int bufferSize = 0;
            _sampleGrabber.GetCurrentBuffer(ref bufferSize, IntPtr.Zero);
            if (bufferSize == 0) return null;

            // Schritt 2: Buffer befüllen
            var buffer = new byte[bufferSize];
            var handle = System.Runtime.InteropServices.GCHandle.Alloc(buffer,
                System.Runtime.InteropServices.GCHandleType.Pinned);

            try
            {
                _sampleGrabber.GetCurrentBuffer(ref bufferSize, handle.AddrOfPinnedObject());

                // Schritt 3: DirectShow Media-Typ auslesen für Auflösung
                var mediaType = new AMMediaType();
                _sampleGrabber.GetConnectedMediaType(mediaType);
                var videoInfo = (VideoInfoHeader)System.Runtime.InteropServices.Marshal.PtrToStructure(
                    mediaType.formatPtr, typeof(VideoInfoHeader))!;

                int width  = videoInfo.BmiHeader.Width;
                int height = Math.Abs(videoInfo.BmiHeader.Height);

                // Schritt 4: RGB24-Buffer → WPF BitmapSource
                // DirectShow liefert das Bild vertikal gespiegelt (bottom-up) → Flip nötig
                var bitmap = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
                var bitmapData = bitmap.LockBits(
                    new System.Drawing.Rectangle(0, 0, width, height),
                    System.Drawing.Imaging.ImageLockMode.WriteOnly,
                    System.Drawing.Imaging.PixelFormat.Format24bppRgb);

                System.Runtime.InteropServices.Marshal.Copy(buffer, 0, bitmapData.Scan0, bufferSize);
                bitmap.UnlockBits(bitmapData);
                bitmap.RotateFlip(System.Drawing.RotateFlipType.RotateNoneFlipY);

                // Schritt 5: System.Drawing.Bitmap → WPF BitmapSource konvertieren
                using var memStream = new System.IO.MemoryStream();
                bitmap.Save(memStream, System.Drawing.Imaging.ImageFormat.Bmp);
                memStream.Seek(0, System.IO.SeekOrigin.Begin);

                var bitmapImage = new BitmapImage();
                bitmapImage.BeginInit();
                bitmapImage.CacheOption  = BitmapCacheOption.OnLoad;
                bitmapImage.StreamSource = memStream;
                bitmapImage.EndInit();
                bitmapImage.Freeze(); // Freeze: thread-sicher für UI-Thread

                return bitmapImage;
            }
            finally
            {
                handle.Free();
            }
        }

        public void Stop()
        {
            _stopRequested = true;
            _mediaControl?.Stop();
            IsRunning = false;
        }

        /// <summary>
        /// Gibt den zuletzt gecapturten Frame zurück – wird vom
        /// CaptureManager beim Speichern verwendet.
        /// Thread-sicher durch Lock.
        /// </summary>
        public BitmapSource? GetCurrentFrame()
        {
            lock (_frameLock)
                return _currentFrame;
        }

        public void Dispose()
        {
            Stop();
            // COM-Objekte freigeben (DirectShow ist COM-basiert)
            if (_filterGraph != null)
                System.Runtime.InteropServices.Marshal.ReleaseComObject(_filterGraph);
        }
    }
}
