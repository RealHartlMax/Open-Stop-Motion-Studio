using Avalonia.Media.Imaging;

namespace OpenStopMotionStudio.Core
{
    public sealed class CapturedFrame
    {
        public CapturedFrame(int index, string shotName, string masterPath, string proxyPath, Bitmap? previewFrame)
        {
            Index = index;
            ShotName = shotName;
            MasterPath = masterPath;
            ProxyPath = proxyPath;
            PreviewFrame = previewFrame;
        }

        public CapturedFrame(int index, string shotName, string masterPath, string proxyPath)
        {
            Index = index;
            ShotName = shotName;
            MasterPath = masterPath;
            ProxyPath = proxyPath;
            PreviewFrame = null;
        }

        public int Index { get; }
        public string ShotName { get; }
        public string MasterPath { get; }
        public string ProxyPath { get; }
        public Bitmap? PreviewFrame { get; set; }
        public string DisplayName => $"{ShotName}_{Index:D4}";
    }
}
