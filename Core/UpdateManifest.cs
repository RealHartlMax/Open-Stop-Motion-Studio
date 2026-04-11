using System.Text.Json.Serialization;
using System.Collections.Generic;

namespace OpenStopMotionStudio.Core
{
    public sealed class UpdateManifest
    {
        [JsonPropertyName("updateInfoUrl")]
        public string? UpdateInfoUrl { get; set; }

        [JsonPropertyName("latestVersion")]
        public string? LatestVersion { get; set; }

        [JsonPropertyName("downloadUrl")]
        public string? DownloadUrl { get; set; }

        [JsonPropertyName("releaseNotes")]
        public string? ReleaseNotes { get; set; }

        [JsonPropertyName("changelog")]
        public List<UpdateChangelogEntry>? Changelog { get; set; }
    }

    public sealed class UpdateChangelogEntry
    {
        [JsonPropertyName("version")]
        public string? Version { get; set; }

        [JsonPropertyName("date")]
        public string? Date { get; set; }

        [JsonPropertyName("title")]
        public string? Title { get; set; }

        [JsonPropertyName("highlights")]
        public List<string>? Highlights { get; set; }
    }
}
