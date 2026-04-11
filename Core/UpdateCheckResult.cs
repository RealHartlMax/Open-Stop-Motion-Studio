namespace OpenStopMotionStudio.Core
{
    public sealed class UpdateCheckResult
    {
        public bool IsUpdateAvailable { get; init; }
        public string CurrentVersion { get; init; } = string.Empty;
        public string LatestVersion { get; init; } = string.Empty;
        public string DownloadUrl { get; init; } = string.Empty;
        public string ReleaseNotes { get; init; } = string.Empty;
    }
}
