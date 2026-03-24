namespace OpenStopMotionStudio.Core
{
    public sealed class NefImportSettings
    {
        public string ProjectFolder { get; init; } = string.Empty;
        public string ShotName { get; init; } = "shot";
        public int FrameStart { get; init; } = 1001;
        public RawImportProxyFormat ProxyFormat { get; init; } = RawImportProxyFormat.Jpeg;
        public int JpegQuality { get; init; } = 92;
    }
}
