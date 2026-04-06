using Avalonia.Media.Imaging;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;

namespace OpenStopMotionStudio.Core
{
    public class CaptureManager
    {
        public int FrameCount { get; private set; }
        public int FrameStart { get; private set; } = 1;
        public Bitmap? LastFrame { get; private set; }
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
                AppContext.BaseDirectory,
                "Projects",
                "Untitled");

            Frames = new ReadOnlyObservableCollection<CapturedFrame>(_frames);
            EnsureOutputFolderExists();
        }

        public CapturedFrame SaveFrame(Bitmap frame)
        {
            int frameNumber = NextFrameNumber;
            string extension = ".png";

            string masterPath = ProjectPaths.BuildOriginalPath(_outputFolder, ShotName, frameNumber, extension);
            string proxyPath = ProjectPaths.BuildProxyPath(_outputFolder, ShotName, frameNumber);

            EnsureParentDirectoryExists(masterPath);
            EnsureParentDirectoryExists(proxyPath);
            
            var clonedFrame = CloneBitmap(frame);

            clonedFrame.Save(masterPath);
            SaveJpeg(clonedFrame, proxyPath, 85);

            FrameCount++;
            LastFrame = clonedFrame;

            var capturedFrame = new CapturedFrame(frameNumber, ShotName, masterPath, proxyPath, clonedFrame);
            _frames.Add(capturedFrame);

            System.Diagnostics.Debug.WriteLine($"[CaptureManager] Saved master: {masterPath}");
            System.Diagnostics.Debug.WriteLine($"[CaptureManager] Saved proxy: {proxyPath}");

            return capturedFrame;
        }

        public CapturedFrame ImportCapturedFile(string sourcePath, Bitmap? previewFallback = null, bool moveSourceFile = false)
        {
            if (string.IsNullOrWhiteSpace(sourcePath) || !File.Exists(sourcePath))
                throw new FileNotFoundException("Die aufgenommene Quelldatei wurde nicht gefunden.", sourcePath);

            int frameNumber = NextFrameNumber;
            string extension = NormalizeExtension(Path.GetExtension(sourcePath));
            string masterPath = ProjectPaths.BuildOriginalPath(_outputFolder, ShotName, frameNumber, extension);

            EnsureParentDirectoryExists(masterPath);
            MoveOrCopyFile(sourcePath, masterPath, moveSourceFile);

            string proxyPath = ProjectPaths.BuildProxyPath(_outputFolder, ShotName, frameNumber);
            var previewFrame = CreatePreviewBitmap(masterPath, previewFallback);

            EnsureParentDirectoryExists(proxyPath);
            SaveJpeg(previewFrame, proxyPath, 85);

            return RegisterCapturedFrame(frameNumber, ShotName, masterPath, proxyPath, previewFrame);
        }

        public ProjectLoadSummary? LoadProjectFramesFromDisk()
        {
            if (!Directory.Exists(_outputFolder))
                return null;

            Dictionary<string, List<ProjectFrameEntry>> framesByShot = new(StringComparer.OrdinalIgnoreCase);

            foreach (string shotDirectory in Directory.EnumerateDirectories(_outputFolder))
            {
                string shotNameFromFolder = Path.GetFileName(shotDirectory);
                
                var originalDirs = Directory.EnumerateDirectories(shotDirectory)
                    .Where(d => Path.GetFileName(d).StartsWith("original_capture_"));

                foreach (string originalDir in originalDirs)
                {
                    foreach (string masterPath in Directory.EnumerateFiles(originalDir))
                    {
                        if (!TryParseFrameFileName(masterPath, out string shotName, out int frameNumber))
                            continue;

                        if (!framesByShot.TryGetValue(shotName, out List<ProjectFrameEntry>? entries))
                        {
                            entries = new List<ProjectFrameEntry>();
                            framesByShot.Add(shotName, entries);
                        }

                        entries.Add(new ProjectFrameEntry(frameNumber, masterPath, File.GetLastWriteTimeUtc(masterPath)));
                    }
                }
            }

            if (framesByShot.Count == 0)
                return null;

            string selectedShot = framesByShot
                .OrderByDescending(pair => pair.Value.Max(entry => entry.LastWriteTimeUtc))
                .ThenBy(pair => pair.Key, StringComparer.OrdinalIgnoreCase)
                .First()
                .Key;

            List<ProjectFrameEntry> selectedEntries = framesByShot[selectedShot]
                .OrderBy(entry => entry.FrameNumber)
                .ToList();

            List<CapturedFrame> loadedFrames = new(selectedEntries.Count);
            foreach (ProjectFrameEntry entry in selectedEntries)
            {
                string proxyPath = ResolveProxyPath(selectedShot, entry.FrameNumber, entry.MasterPath);
                Bitmap previewFrame = LoadPreviewBitmap(entry.MasterPath, proxyPath);
                loadedFrames.Add(new CapturedFrame(entry.FrameNumber, selectedShot, entry.MasterPath, proxyPath, previewFrame));
            }

            int frameStart = selectedEntries.Count > 0 ? selectedEntries[0].FrameNumber : 1;
            return new ProjectLoadSummary(selectedShot, frameStart, loadedFrames);
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
            string shotName = NormalizeShotName(ShotName);
            int frameNumber = NextFrameNumber;
            return $"Shot: {shotName}, Frame: {frameNumber:D5}";
        }

        public string GetWorkflowDescription()
        {
            return "Originals in \\<Shot>\\original_capture_*, Proxies in \\<Shot>\\proxy";
        }

        private static Bitmap CloneBitmap(Bitmap original)
        {
            using (var memoryStream = new MemoryStream())
            {
                original.Save(memoryStream);
                memoryStream.Position = 0;
                return new Bitmap(memoryStream);
            }
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

        private static void DeleteIfExists(string path)
        {
            if (File.Exists(path))
                File.Delete(path);
        }

        private CapturedFrame RegisterCapturedFrame(int frameNumber, string shotName, string masterPath, string proxyPath, Bitmap previewFrame)
        {
            Bitmap storedPreview = CloneBitmap(previewFrame);
            FrameCount++;
            LastFrame = storedPreview;

            var capturedFrame = new CapturedFrame(frameNumber, shotName, masterPath, proxyPath, storedPreview);
            _frames.Add(capturedFrame);

            System.Diagnostics.Debug.WriteLine($"[CaptureManager] Registered original: {masterPath}");
            if (!string.Equals(masterPath, proxyPath, StringComparison.OrdinalIgnoreCase))
                System.Diagnostics.Debug.WriteLine($"[CaptureManager] Registered proxy: {proxyPath}");

            return capturedFrame;
        }

        private string ResolveProxyPath(string shotName, int frameNumber, string masterPath)
        {
            string proxyPath = ProjectPaths.BuildProxyPath(_outputFolder, shotName, frameNumber);
            if (File.Exists(proxyPath))
                return proxyPath;

            return masterPath; // Fallback to master if proxy is not found
        }

        private static Bitmap LoadPreviewBitmap(string masterPath, string proxyPath)
        {
            foreach (string candidate in new[] { proxyPath, masterPath })
            {
                if (!File.Exists(candidate))
                    continue;
                try
                {
                    return new Bitmap(candidate);
                }
                catch
                {
                    // continue with next candidate
                }
            }
            throw new InvalidOperationException($"Es konnte kein Vorschaubild für {Path.GetFileName(masterPath)} geladen werden.");
        }

        private static Bitmap CreatePreviewBitmap(string masterPath, Bitmap? previewFallback)
        {
            if (previewFallback is not null)
                return CloneBitmap(previewFallback);

            try
            {
                // For non-bitmap formats, we can't create a preview here without a library.
                // This will likely rely on the camera providing a preview.
                // For now, we assume it's a format Bitmap can handle or a fallback is provided.
                return new Bitmap(masterPath);
            }
            catch (Exception ex)
            {
                throw new InvalidOperationException(
                    $"Das aufgenommene Bild {Path.GetFileName(masterPath)} konnte nicht als Vorschau geladen werden.",
                    ex);
            }
        }

        private static void SaveJpeg(Bitmap bitmap, string path, int quality)
        {
            using var fileStream = new FileStream(path, FileMode.Create, FileAccess.Write);
            bitmap.Save(fileStream, quality);
        }

        private static bool IsPreviewNativeExtension(string extension)
        {
            return extension.Equals(".jpg", StringComparison.OrdinalIgnoreCase)
                || extension.Equals(".jpeg", StringComparison.OrdinalIgnoreCase)
                || extension.Equals(".png", StringComparison.OrdinalIgnoreCase);
        }

        private static string NormalizeExtension(string extension)
        {
            if (string.IsNullOrWhiteSpace(extension))
                return ".dat";

            return extension.StartsWith(".", StringComparison.Ordinal) ? extension : "." + extension;
        }

        private static void MoveOrCopyFile(string sourcePath, string targetPath, bool moveSourceFile)
        {
            if (string.Equals(sourcePath, targetPath, StringComparison.OrdinalIgnoreCase))
                return;

            if (moveSourceFile)
            {
                if (File.Exists(targetPath))
                    File.Delete(targetPath);

                File.Move(sourcePath, targetPath);
                return;
            }

            File.Copy(sourcePath, targetPath, overwrite: true);
        }

        private static bool TryParseFrameFileName(string path, out string shotName, out int frameNumber)
        {
            shotName = string.Empty;
            frameNumber = 0;

            string fileName = Path.GetFileNameWithoutExtension(path);
            if (string.IsNullOrWhiteSpace(fileName))
                return false;

            int separatorIndex = fileName.LastIndexOf('_');
            if (separatorIndex <= 0 || separatorIndex >= fileName.Length - 1)
                return false;

            string indexPart = fileName[(separatorIndex + 1)..];
            if (!int.TryParse(indexPart, out frameNumber) || frameNumber <= 0)
                return false;

            shotName = NormalizeShotName(fileName[..separatorIndex]);
            return !string.IsNullOrWhiteSpace(shotName);
        }

        private sealed class ProjectFrameEntry
        {
            public ProjectFrameEntry(int frameNumber, string masterPath, DateTime lastWriteTimeUtc)
            {
                FrameNumber = frameNumber;
                MasterPath = masterPath;
                LastWriteTimeUtc = lastWriteTimeUtc;
            }

            public int FrameNumber { get; }
            public string MasterPath { get; }
            public DateTime LastWriteTimeUtc { get; }
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
