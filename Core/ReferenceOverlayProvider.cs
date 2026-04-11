using Avalonia.Media.Imaging;
using Emgu.CV;
using Emgu.CV.CvEnum;
using Emgu.CV.Util;
using System.Collections.Generic;
using System.IO;

namespace OpenStopMotionStudio.Core
{
    /// <summary>
    /// Provides a frame-based reference overlay source for live stop-motion alignment.
    /// Supports single image, image sequence and video files.
    /// </summary>
    public sealed class ReferenceOverlayProvider : IDisposable
    {
        private const int MaxVideoFrameCacheEntries = 48;
        private const int MaxSequenceFrameCacheEntries = 48;

        private static readonly HashSet<string> ImageExtensions = new(StringComparer.OrdinalIgnoreCase)
        {
            ".png", ".jpg", ".jpeg", ".bmp", ".gif", ".webp"
        };

        private readonly Dictionary<int, Bitmap> _videoFrameCache = new();
        private readonly LinkedList<int> _videoCacheOrder = new();
        private readonly Dictionary<int, Bitmap> _sequenceFrameCache = new();
        private readonly LinkedList<int> _sequenceCacheOrder = new();
        private readonly object _sync = new();

        private Bitmap? _singleImageBitmap;
        private List<string> _sequencePaths = new();
        private VideoCapture? _videoCapture;
        private int _videoFrameCount;

        public ReferenceOverlaySource Source { get; private set; } = ReferenceOverlaySource.None;

        public ReferenceOverlayPlaybackMode PlaybackMode { get; set; } = ReferenceOverlayPlaybackMode.Loop;

        public bool HasSource => Source != ReferenceOverlaySource.None;

        public int FrameCount => Source switch
        {
            ReferenceOverlaySource.SingleImage => _singleImageBitmap is null ? 0 : 1,
            ReferenceOverlaySource.ImageSequence => _sequencePaths.Count,
            ReferenceOverlaySource.Video => Math.Max(0, _videoFrameCount),
            _ => 0
        };

        public string SourceLabel { get; private set; } = "Keine Referenz geladen";

        public void Clear()
        {
            lock (_sync)
            {
                DisposeVideo();
                DisposeSingleImage();
                DisposeVideoCache();
                DisposeSequenceCache();
                _sequencePaths = new List<string>();
                Source = ReferenceOverlaySource.None;
                SourceLabel = "Keine Referenz geladen";
            }
        }

        public void LoadSingleImage(string filePath)
        {
            if (string.IsNullOrWhiteSpace(filePath) || !File.Exists(filePath))
                throw new FileNotFoundException("Overlay-Bild nicht gefunden.", filePath);

            lock (_sync)
            {
                Clear();
                using var stream = File.OpenRead(filePath);
                _singleImageBitmap = new Bitmap(stream);
                Source = ReferenceOverlaySource.SingleImage;
                SourceLabel = $"Bild: {Path.GetFileName(filePath)}";
            }
        }

        public void LoadImageSequence(string folderPath)
        {
            if (string.IsNullOrWhiteSpace(folderPath) || !Directory.Exists(folderPath))
                throw new DirectoryNotFoundException("Overlay-Ordner nicht gefunden.");

            var sequenceFiles = Directory.GetFiles(folderPath)
                .Where(path => ImageExtensions.Contains(Path.GetExtension(path)))
                .OrderBy(path => path, StringComparer.OrdinalIgnoreCase)
                .ToList();

            if (sequenceFiles.Count == 0)
                throw new InvalidOperationException("Im gewählten Ordner wurden keine unterstützten Bilddateien gefunden.");

            lock (_sync)
            {
                Clear();
                _sequencePaths = sequenceFiles;
                Source = ReferenceOverlaySource.ImageSequence;
                SourceLabel = $"Sequenz: {Path.GetFileName(folderPath)} ({_sequencePaths.Count} Frames)";
            }
        }

        public void LoadVideo(string filePath)
        {
            if (string.IsNullOrWhiteSpace(filePath) || !File.Exists(filePath))
                throw new FileNotFoundException("Overlay-Video nicht gefunden.", filePath);

            lock (_sync)
            {
                Clear();
                _videoCapture = new VideoCapture(filePath);
                if (!_videoCapture.IsOpened)
                {
                    DisposeVideo();
                    throw new InvalidOperationException("Das Overlay-Video konnte nicht geöffnet werden.");
                }

                _videoFrameCount = Math.Max(0, (int)_videoCapture.Get(CapProp.FrameCount));
                if (_videoFrameCount == 0)
                {
                    DisposeVideo();
                    throw new InvalidOperationException("Das Overlay-Video enthält keine lesbaren Frames.");
                }

                Source = ReferenceOverlaySource.Video;
                SourceLabel = $"Video: {Path.GetFileName(filePath)} ({_videoFrameCount} Frames)";
            }
        }

        public Bitmap? GetFrameForTimelineFrame(int frameNumber)
        {
            if (frameNumber < 1)
                frameNumber = 1;

            lock (_sync)
            {
                return Source switch
                {
                    ReferenceOverlaySource.SingleImage => _singleImageBitmap,
                    ReferenceOverlaySource.ImageSequence => LoadSequenceFrame(frameNumber),
                    ReferenceOverlaySource.Video => LoadVideoFrame(frameNumber),
                    _ => null
                };
            }
        }

        private Bitmap? LoadSequenceFrame(int frameNumber)
        {
            if (_sequencePaths.Count == 0)
                return null;

            int index = ResolveFrameIndex(frameNumber, _sequencePaths.Count);
            if (index < 0)
                return null;

            if (_sequenceFrameCache.TryGetValue(index, out Bitmap? cached))
                return cached;

            string path = _sequencePaths[index];
            using var stream = File.OpenRead(path);
            Bitmap bitmap = new(stream);

            _sequenceFrameCache[index] = bitmap;
            _sequenceCacheOrder.AddLast(index);
            TrimSequenceCache();

            return bitmap;
        }

        private Bitmap? LoadVideoFrame(int frameNumber)
        {
            if (_videoCapture is null || _videoFrameCount <= 0)
                return null;

            int index = ResolveFrameIndex(frameNumber, _videoFrameCount);
            if (index < 0)
                return null;

            if (_videoFrameCache.TryGetValue(index, out Bitmap? cached))
                return cached;

            using var frame = new Mat();
            using var bgra = new Mat();
            using var buffer = new VectorOfByte();

            _videoCapture.Set(CapProp.PosFrames, index);
            if (!_videoCapture.Read(frame) || frame.IsEmpty)
                return null;

            CvInvoke.CvtColor(frame, bgra, ColorConversion.Bgr2Bgra);
            CvInvoke.Imencode(".png", bgra, buffer);

            using var stream = new MemoryStream(buffer.ToArray());
            Bitmap bitmap = new(stream);

            _videoFrameCache[index] = bitmap;
            _videoCacheOrder.AddLast(index);
            TrimVideoCache();

            return bitmap;
        }

        private int ResolveFrameIndex(int frameNumber, int frameCount)
        {
            if (frameCount <= 0)
                return -1;

            int zeroBased = Math.Max(0, frameNumber - 1);
            return PlaybackMode switch
            {
                ReferenceOverlayPlaybackMode.HoldLastFrame => Math.Min(zeroBased, frameCount - 1),
                _ => zeroBased % frameCount
            };
        }

        private void TrimVideoCache()
        {
            while (_videoCacheOrder.Count > MaxVideoFrameCacheEntries)
            {
                int oldestIndex = _videoCacheOrder.First!.Value;
                _videoCacheOrder.RemoveFirst();
                if (_videoFrameCache.Remove(oldestIndex, out Bitmap? oldBitmap))
                    oldBitmap.Dispose();
            }
        }

        private void TrimSequenceCache()
        {
            while (_sequenceCacheOrder.Count > MaxSequenceFrameCacheEntries)
            {
                int oldestIndex = _sequenceCacheOrder.First!.Value;
                _sequenceCacheOrder.RemoveFirst();
                if (_sequenceFrameCache.Remove(oldestIndex, out Bitmap? oldBitmap))
                    oldBitmap.Dispose();
            }
        }

        private void DisposeSingleImage()
        {
            _singleImageBitmap?.Dispose();
            _singleImageBitmap = null;
        }

        private void DisposeVideo()
        {
            _videoCapture?.Dispose();
            _videoCapture = null;
            _videoFrameCount = 0;
        }

        private void DisposeVideoCache()
        {
            foreach (Bitmap bitmap in _videoFrameCache.Values)
                bitmap.Dispose();

            _videoFrameCache.Clear();
            _videoCacheOrder.Clear();
        }

        private void DisposeSequenceCache()
        {
            foreach (Bitmap bitmap in _sequenceFrameCache.Values)
                bitmap.Dispose();

            _sequenceFrameCache.Clear();
            _sequenceCacheOrder.Clear();
        }

        public static bool IsSupportedImageFile(string filePath)
        {
            return ImageExtensions.Contains(Path.GetExtension(filePath));
        }

        public void Dispose()
        {
            Clear();
        }
    }
}
