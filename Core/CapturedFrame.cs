using System.Windows.Media.Imaging;

namespace OpenStopMotionStudio.Core
{
    public sealed class CapturedFrame
    {
        public CapturedFrame(int index, string shotName, string masterPath, string proxyPath, BitmapSource previewFrame)
        {
            Index = index;
            ShotName = shotName;
            MasterPath = masterPath;
            ProxyPath = proxyPath;
            PreviewFrame = previewFrame;
        }

        public int Index { get; }
        public string ShotName { get; }
        public string MasterPath { get; }
        public string ProxyPath { get; }
        public BitmapSource PreviewFrame { get; }
        public string DisplayName => $"{ShotName}_{Index:D4}";
    }
}
