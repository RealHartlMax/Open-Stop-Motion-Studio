using System.Collections.Generic;

namespace OpenStopMotionStudio.Core
{
    public sealed class ProjectLoadSummary
    {
        public ProjectLoadSummary(string shotName, int frameStart, IReadOnlyList<CapturedFrame> frames)
        {
            ShotName = shotName;
            FrameStart = frameStart;
            Frames = frames;
        }

        public string ShotName { get; }
        public int FrameStart { get; }
        public IReadOnlyList<CapturedFrame> Frames { get; }
        public int LoadedFrameCount => Frames.Count;
    }
}
