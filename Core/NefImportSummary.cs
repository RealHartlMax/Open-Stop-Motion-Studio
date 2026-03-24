using System.Collections.Generic;

namespace OpenStopMotionStudio.Core
{
    public sealed class NefImportSummary
    {
        public NefImportSummary(
            string shotName,
            int frameStart,
            string masterFolder,
            string proxyFolder,
            IReadOnlyList<CapturedFrame> importedFrames)
        {
            ShotName = shotName;
            FrameStart = frameStart;
            MasterFolder = masterFolder;
            ProxyFolder = proxyFolder;
            ImportedFrames = importedFrames;
        }

        public string ShotName { get; }
        public int FrameStart { get; }
        public string MasterFolder { get; }
        public string ProxyFolder { get; }
        public IReadOnlyList<CapturedFrame> ImportedFrames { get; }
        public int ImportedCount => ImportedFrames.Count;
    }
}
