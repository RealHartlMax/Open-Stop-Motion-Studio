using System;
using System.Linq;
using System.IO;
using System.Net.Http;
using System.Reflection;
using System.Text.Json;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.Core
{
    public sealed class UpdateService
    {
        private static readonly JsonSerializerOptions JsonOptions = new()
        {
            PropertyNameCaseInsensitive = true
        };

        public async Task<UpdateCheckResult?> CheckForUpdatesAsync()
        {
            try
            {
                string appVersion = GetCurrentVersion();
                UpdateManifest? localManifest = await LoadManifestFromDiskAsync();
                if (localManifest == null)
                    return null;

                UpdateManifest manifest = localManifest;
                if (!string.IsNullOrWhiteSpace(localManifest.UpdateInfoUrl))
                {
                    UpdateManifest? remoteManifest = await LoadManifestFromUrlAsync(localManifest.UpdateInfoUrl!);
                    if (remoteManifest != null)
                        manifest = remoteManifest;
                }

                string latestVersion = NormalizeVersion(manifest.LatestVersion);
                if (string.IsNullOrWhiteSpace(latestVersion))
                    return null;

                bool updateAvailable = TryParseVersion(latestVersion, out Version? latest)
                    && TryParseVersion(appVersion, out Version? current)
                    && latest > current;

                return new UpdateCheckResult
                {
                    IsUpdateAvailable = updateAvailable,
                    CurrentVersion = appVersion,
                    LatestVersion = latestVersion,
                    DownloadUrl = manifest.DownloadUrl ?? string.Empty,
                    ReleaseNotes = BuildReleaseNotes(manifest, latestVersion)
                };
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogInfo("UpdateService", $"Update check failed: {ex.Message}");
                return null;
            }
        }

        private static async Task<UpdateManifest?> LoadManifestFromDiskAsync()
        {
            string[] candidates =
            {
                Path.Combine(AppContext.BaseDirectory, "versions.json"),
                Path.Combine(ProjectRoot.GetPath(), "versions.json")
            };

            foreach (string filePath in candidates)
            {
                if (!File.Exists(filePath))
                    continue;

                try
                {
                    string json = await File.ReadAllTextAsync(filePath);
                    UpdateManifest? manifest = JsonSerializer.Deserialize<UpdateManifest>(json, JsonOptions);
                    if (manifest != null)
                        return manifest;
                }
                catch (Exception ex)
                {
                    DebugLogger.Instance.LogInfo("UpdateService", $"Invalid update manifest '{filePath}': {ex.Message}");
                }
            }

            return null;
        }

        private static async Task<UpdateManifest?> LoadManifestFromUrlAsync(string url)
        {
            try
            {
                using var client = new HttpClient { Timeout = TimeSpan.FromSeconds(5) };
                string json = await client.GetStringAsync(url);
                return JsonSerializer.Deserialize<UpdateManifest>(json, JsonOptions);
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogInfo("UpdateService", $"Remote update manifest unavailable: {ex.Message}");
                return null;
            }
        }

        private static string GetCurrentVersion()
        {
            Version? version = Assembly.GetExecutingAssembly().GetName().Version;
            if (version == null)
                return "0.0.0";

            int patch = version.Build >= 0 ? version.Build : 0;
            return $"{version.Major}.{version.Minor}.{patch}";
        }

        private static string NormalizeVersion(string? value)
        {
            if (string.IsNullOrWhiteSpace(value))
                return string.Empty;

            string normalized = value.Trim();
            if (normalized.StartsWith("v", StringComparison.OrdinalIgnoreCase))
                normalized = normalized[1..];

            return normalized;
        }

        private static bool TryParseVersion(string value, out Version? version)
        {
            version = null;
            string[] segments = value.Split('.', StringSplitOptions.RemoveEmptyEntries);
            if (segments.Length == 2)
                value += ".0";

            return Version.TryParse(value, out version);
        }

        private static string BuildReleaseNotes(UpdateManifest manifest, string latestVersion)
        {
            if (!string.IsNullOrWhiteSpace(manifest.ReleaseNotes))
                return manifest.ReleaseNotes!;

            if (manifest.Changelog == null || manifest.Changelog.Count == 0)
                return string.Empty;

            UpdateChangelogEntry? latestEntry = manifest.Changelog
                .FirstOrDefault(entry => string.Equals(NormalizeVersion(entry.Version), latestVersion, StringComparison.OrdinalIgnoreCase));

            if (latestEntry == null)
                return string.Empty;

            var highlights = latestEntry.Highlights ?? [];
            if (highlights.Count == 0)
                return latestEntry.Title ?? string.Empty;

            return string.Join(Environment.NewLine, highlights.Select(static item => $"- {item}"));
        }
    }
}
