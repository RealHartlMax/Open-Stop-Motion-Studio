using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Media.Imaging;

namespace OpenStopMotionStudio.Core
{
    /// <summary>
    /// CaptureManager: Verantwortlich für das Speichern von Frames und
    /// die Verwaltung der Bildsequenz.
    /// </summary>
    public class CaptureManager
    {
        public int FrameCount { get; private set; }
        public int FrameStart { get; private set; } = 1;
        public BitmapSource? LastFrame { get; private set; }
        public string ShotName { get; private set; } = "frame";
        public CaptureOutputMode OutputMode { get; private set; } = CaptureOutputMode.JpegSequence;
        public string OutputFolder => _outputFolder;
        public int LastFrameNumber => _frames.Count > 0 ? _frames[^1].Index : 0;
        public int NextFrameNumber => FrameStart + FrameCount;
        public ReadOnlyObservableCollection<CapturedFrame> Frames { get; }

        private readonly ObservableCollection<CapturedFrame> _frames = new();
        private string _outputFolder;

        public CaptureManager()
        {
            _outputFolder = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                "OpenStopMotionStudio",
                "Untitled");

            Frames = new ReadOnlyObservableCollection<CapturedFrame>(_frames);
            EnsureOutputFolderExists();
        }

        public CapturedFrame SaveFrame(BitmapSource frame)
        {
            int frameNumber = NextFrameNumber;

            string masterPath = BuildMasterPath(frameNumber);
            string proxyPath = BuildProxyPath(frameNumber);

            EnsureParentDirectoryExists(masterPath);
            EnsureParentDirectoryExists(proxyPath);

            switch (OutputMode)
            {
                case CaptureOutputMode.TiffWithProxy:
                    SaveBitmap(frame, masterPath, new TiffBitmapEncoder());
                    SaveBitmap(frame, proxyPath, new JpegBitmapEncoder { QualityLevel = 85 });
                    break;

                default:
                    SaveBitmap(frame, masterPath, new JpegBitmapEncoder { QualityLevel = 90 });
                    proxyPath = masterPath;
                    break;
            }

            FrameCount++;
            LastFrame = frame;

            var capturedFrame = new CapturedFrame(frameNumber, ShotName, masterPath, proxyPath, frame);
            _frames.Add(capturedFrame);

            System.Diagnostics.Debug.WriteLine($"[CaptureManager] Saved master: {masterPath}");
            if (!string.Equals(masterPath, proxyPath, StringComparison.OrdinalIgnoreCase))
                System.Diagnostics.Debug.WriteLine($"[CaptureManager] Saved proxy: {proxyPath}");

            return capturedFrame;
        }

        public void SetOutputFolder(string folder)
        {
            _outputFolder = folder;
            EnsureOutputFolderExists();
        }

        public void SetOutputMode(CaptureOutputMode mode)
        {
            OutputMode = mode;
        }

        public void NewProject()
        {
            FrameCount = 0;
            LastFrame = null;
            _frames.Clear();
        }

        public string BeginShot(string shotName, int frameStart = 1)
        {
            ShotName = NormalizeShotName(shotName);
            FrameStart = Math.Max(1, frameStart);
            NewProject();
            return ShotName;
        }

        public void LoadImportedFrames(string shotName, int frameStart, IEnumerable<CapturedFrame> frames)
        {
            ShotName = NormalizeShotName(shotName);
            FrameStart = Math.Max(1, frameStart);
            _frames.Clear();

            foreach (CapturedFrame frame in frames.OrderBy(frame => frame.Index))
                _frames.Add(frame);

            FrameCount = _frames.Count;
            LastFrame = _frames.Count > 0 ? _frames[^1].PreviewFrame : null;
        }

        public bool UndoLastCapture()
        {
            if (_frames.Count == 0)
                return false;

            var frame = _frames[^1];

            DeleteIfExists(frame.MasterPath);
            if (!string.Equals(frame.MasterPath, frame.ProxyPath, StringComparison.OrdinalIgnoreCase))
                DeleteIfExists(frame.ProxyPath);

            _frames.RemoveAt(_frames.Count - 1);
            FrameCount--;
            LastFrame = _frames.Count > 0 ? _frames[^1].PreviewFrame : null;

            return true;
        }

        public IReadOnlyList<CapturedFrame> GetRecentFrames(int maxCount)
        {
            if (maxCount <= 0 || _frames.Count == 0)
                return Array.Empty<CapturedFrame>();

            return _frames
                .Reverse()
                .Take(maxCount)
                .ToList();
        }

        public string GetNextCapturePreview()
        {
            string masterName = GetMasterFileName(NextFrameNumber);

            return OutputMode switch
            {
                CaptureOutputMode.TiffWithProxy => $"Master: {masterName} | Proxy: {GetProxyFileName(NextFrameNumber)}",
                _ => $"Nächste Datei: {masterName}"
            };
        }

        public string GetWorkflowDescription()
        {
            return OutputMode switch
            {
                CaptureOutputMode.TiffWithProxy => "TIFF-Master in \\masters, JPEG-Proxies in \\proxy",
                _ => "Direkte JPEG-Sequenz ohne getrennte Proxy-Dateien"
            };
        }

        private void EnsureOutputFolderExists()
        {
            if (!Directory.Exists(_outputFolder))
                Directory.CreateDirectory(_outputFolder);
        }

        private static void EnsureParentDirectoryExists(string path)
        {
            string? directory = Path.GetDirectoryName(path);
            if (!string.IsNullOrWhiteSpace(directory) && !Directory.Exists(directory))
                Directory.CreateDirectory(directory);
        }

        private string BuildMasterPath(int frameIndex)
        {
            return OutputMode switch
            {
                CaptureOutputMode.TiffWithProxy => Path.Combine(_outputFolder, "masters", GetMasterFileName(frameIndex)),
                _ => Path.Combine(_outputFolder, GetMasterFileName(frameIndex))
            };
        }

        private string BuildProxyPath(int frameIndex)
        {
            return OutputMode switch
            {
                CaptureOutputMode.TiffWithProxy => Path.Combine(_outputFolder, "proxy", GetProxyFileName(frameIndex)),
                _ => Path.Combine(_outputFolder, GetMasterFileName(frameIndex))
            };
        }

        private string GetMasterFileName(int frameIndex)
        {
            string extension = OutputMode == CaptureOutputMode.TiffWithProxy ? ".tif" : ".jpg";
            return $"{ShotName}_{frameIndex:D4}{extension}";
        }

        private string GetProxyFileName(int frameIndex) => $"{ShotName}_{frameIndex:D4}.jpg";

        private static void SaveBitmap(BitmapSource bitmap, string path, BitmapEncoder encoder)
        {
            encoder.Frames.Add(BitmapFrame.Create(bitmap));

            using var fileStream = new FileStream(path, FileMode.Create, FileAccess.Write);
            encoder.Save(fileStream);
        }

        private static void DeleteIfExists(string path)
        {
            if (File.Exists(path))
                File.Delete(path);
        }

        public static string NormalizeShotName(string? shotName)
        {
            if (string.IsNullOrWhiteSpace(shotName))
                return "frame";

            var builder = new StringBuilder(shotName.Length);
            foreach (char ch in shotName.Trim())
            {
                if (char.IsLetterOrDigit(ch) || ch == '-' || ch == '_')
                    builder.Append(ch);
                else if (char.IsWhiteSpace(ch))
                    builder.Append('_');
            }

            return builder.Length > 0 ? builder.ToString() : "frame";
        }
    }
}
