using System.Collections.Generic;

namespace OpenStopMotionStudio.Core
{
    public sealed class NefImportFailure
    {
        public NefImportFailure(string sourcePath, string fileName, string error)
        {
            SourcePath = sourcePath;
            FileName = fileName;
            Error = error;
        }

        public string SourcePath { get; }
        public string FileName { get; }
        public string Error { get; }
    }

    public sealed class NefImportSummary
    {
        public NefImportSummary(
            string shotName,
            int frameStart,
            string masterFolder,
            string proxyFolder,
            IReadOnlyList<CapturedFrame> importedFrames,
            IReadOnlyList<NefImportFailure> failedFiles,
            bool wasCanceled)
        {
            ShotName = shotName;
            FrameStart = frameStart;
            MasterFolder = masterFolder;
            ProxyFolder = proxyFolder;
            ImportedFrames = importedFrames;
            FailedFiles = failedFiles;
            WasCanceled = wasCanceled;
        }

        public string ShotName { get; }
        public int FrameStart { get; }
        public string MasterFolder { get; }
        public string ProxyFolder { get; }
        public IReadOnlyList<CapturedFrame> ImportedFrames { get; }
        public IReadOnlyList<NefImportFailure> FailedFiles { get; }
        public bool WasCanceled { get; }
        public int ImportedCount => ImportedFrames.Count;
        public int FailedCount => FailedFiles.Count;
        public bool HasFailures => FailedCount > 0;
    }
}
