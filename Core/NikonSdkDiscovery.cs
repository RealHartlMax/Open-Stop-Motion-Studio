using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;

namespace OpenStopMotionStudio.Core
{
    public sealed class NikonSdkDiscovery
    {
        public NikonImageSdkLocation? FindImageSdk()
        {
            List<NikonImageSdkLocation> candidates = new();

            foreach (string root in EnumerateCandidateRoots().Distinct(StringComparer.OrdinalIgnoreCase))
            {
                string nikonRoot = Path.Combine(root, "SDKs", "Nikon");
                if (!Directory.Exists(nikonRoot))
                    continue;

                try
                {
                    foreach (string libraryPath in Directory.EnumerateFiles(nikonRoot, "NkImgSDK.dll", SearchOption.AllDirectories))
                    {
                        string? libraryDirectory = Path.GetDirectoryName(libraryPath);
                        if (string.IsNullOrWhiteSpace(libraryDirectory))
                            continue;

                        if (!libraryDirectory.Contains($"{Path.DirectorySeparatorChar}Image SDK{Path.DirectorySeparatorChar}", StringComparison.OrdinalIgnoreCase))
                            continue;

                        if (!libraryDirectory.Contains($"{Path.DirectorySeparatorChar}x64{Path.DirectorySeparatorChar}", StringComparison.OrdinalIgnoreCase))
                            continue;

                        string sdkRoot = FindSdkRoot(libraryPath) ?? libraryDirectory;
                        candidates.Add(new NikonImageSdkLocation(sdkRoot, libraryDirectory, libraryPath));
                    }
                }
                catch (Exception ex)
                {
                    DebugLogger.Instance.LogInfo("NikonSdkDiscovery", $"Failed to scan for Nikon Image SDK in '{nikonRoot}': {ex.Message}");
                }
            }

            return candidates
                .OrderByDescending(location => location.LibraryDirectory.Contains($"{Path.DirectorySeparatorChar}Release", StringComparison.OrdinalIgnoreCase))
                .ThenByDescending(location => location.SdkRoot.Contains("S-SDKNEF", StringComparison.OrdinalIgnoreCase))
                .ThenBy(location => location.LibraryPath, StringComparer.OrdinalIgnoreCase)
                .FirstOrDefault();
        }

        public IReadOnlyList<NikonMaidSdkLocation> FindMaidSdks()
        {
            if (!OperatingSystem.IsWindows())
                return Array.Empty<NikonMaidSdkLocation>();

            List<NikonMaidSdkLocation> candidates = new();
            HashSet<string> seenKeys = new(StringComparer.OrdinalIgnoreCase);

            foreach (string root in EnumerateCandidateRoots().Distinct(StringComparer.OrdinalIgnoreCase))
            {
                foreach (string searchRoot in EnumerateNikonSearchRoots(root))
                {
                    if (!Directory.Exists(searchRoot))
                        continue;

                    try
                    {
                        foreach (string modulePath in Directory.EnumerateFiles(searchRoot, "*.md3", SearchOption.AllDirectories))
                        {
                            string? moduleDirectory = Path.GetDirectoryName(modulePath);
                            if (string.IsNullOrWhiteSpace(moduleDirectory))
                                continue;

                            if (!moduleDirectory.Contains($"{Path.DirectorySeparatorChar}Module{Path.DirectorySeparatorChar}Win{Path.DirectorySeparatorChar}Binary Files{Path.DirectorySeparatorChar}x64", StringComparison.OrdinalIgnoreCase))
                                continue;

                            if (!File.Exists(Path.Combine(moduleDirectory, "NkdPTP.dll")))
                                continue;

                            string sdkRoot = FindSdkRoot(modulePath) ?? moduleDirectory;
                            string relativeModulePath = Path.GetRelativePath(sdkRoot, modulePath).Replace('\\', '/');
                            if (!seenKeys.Add($"{sdkRoot}|{relativeModulePath}"))
                                continue;

                            candidates.Add(new NikonMaidSdkLocation(sdkRoot, isArchive: false, relativeModulePath));
                        }

                        foreach (string archivePath in Directory.EnumerateFiles(searchRoot, "S-SDK*.zip", SearchOption.TopDirectoryOnly))
                        {
                            if (!TryFindArchiveModule(archivePath, out string? moduleEntryPath))
                                continue;

                            if (!seenKeys.Add($"{archivePath}|{moduleEntryPath}"))
                                continue;

                            candidates.Add(new NikonMaidSdkLocation(archivePath, isArchive: true, moduleEntryPath!));
                        }
                    }
                    catch (Exception ex)
                    {
                        DebugLogger.Instance.LogInfo("NikonSdkDiscovery", $"Failed to scan for Nikon SDKs in '{searchRoot}': {ex.Message}");
                    }
                }
            }

            return candidates
                .OrderByDescending(ScoreMaidLocation)
                .ThenBy(location => location.SourcePath, StringComparer.OrdinalIgnoreCase)
                .ToList();
        }

        public NikonMaidSdkLocation? FindBestMaidSdk(string? deviceName)
        {
            IReadOnlyList<NikonMaidSdkLocation> candidates = FindMaidSdks();
            if (candidates.Count == 0)
                return null;

            string normalizedDeviceName = NormalizeKey(deviceName);
            return candidates
                .OrderByDescending(location => ScoreDeviceMatch(location, normalizedDeviceName))
                .ThenByDescending(ScoreMaidLocation)
                .ThenBy(location => location.SourcePath, StringComparer.OrdinalIgnoreCase)
                .FirstOrDefault();
        }

        public NikonMaidSdkLocation? ResolveMaidSdk(string? connectionToken, string? deviceName)
        {
            IReadOnlyList<NikonMaidSdkLocation> candidates = FindMaidSdks();
            if (candidates.Count == 0)
                return null;

            if (!string.IsNullOrWhiteSpace(connectionToken))
            {
                NikonMaidSdkLocation? exact = candidates.FirstOrDefault(location =>
                    string.Equals(location.SourcePath, connectionToken, StringComparison.OrdinalIgnoreCase));

                if (exact is not null)
                    return exact;
            }

            string normalizedDeviceName = NormalizeKey(deviceName);
            return candidates
                .OrderByDescending(location =>
                    !string.IsNullOrWhiteSpace(connectionToken)
                    && string.Equals(location.SourcePath, connectionToken, StringComparison.OrdinalIgnoreCase)
                        ? int.MaxValue
                        : ScoreDeviceMatch(location, normalizedDeviceName))
                .ThenByDescending(ScoreMaidLocation)
                .ThenBy(location => location.SourcePath, StringComparer.OrdinalIgnoreCase)
                .FirstOrDefault();
        }

        public IReadOnlyList<NikonMaidDevice> EnumerateConnectedMaidCameras()
        {
            if (!OperatingSystem.IsWindows())
                return Array.Empty<NikonMaidDevice>();

            List<NikonMaidDevice> devices = new();
            HashSet<string> seenNames = new(StringComparer.OrdinalIgnoreCase);

            foreach (NikonMaidSdkLocation location in FindMaidSdks())
            {
                try
                {
                    using var session = NikonMaidSession.Open(location, null);
                    string deviceName = string.IsNullOrWhiteSpace(session.SourceName)
                        ? $"Nikon Kamera ({location.DisplayName})"
                        : session.SourceName.Trim();

                    string dedupeKey = NormalizeKey(deviceName);
                    if (!seenNames.Add(dedupeKey))
                        continue;

                    devices.Add(new NikonMaidDevice(deviceName, location.SourcePath));
                }
                catch
                {
                    // Ignore SDK packages that do not match a connected camera.
                }
            }

            return devices;
        }

        private static IEnumerable<string> EnumerateCandidateRoots()
        {
            yield return ProjectRoot.GetPath();
            yield return AppContext.BaseDirectory;
        }

        private static IEnumerable<string> EnumerateNikonSearchRoots(string root)
        {
            yield return root;

            string nikonRoot = Path.Combine(root, "SDKs", "Nikon");
            if (Directory.Exists(nikonRoot))
                yield return nikonRoot;

            if (!Directory.Exists(root))
                yield break;

            foreach (string childDirectory in Directory.EnumerateDirectories(root))
            {
                string directoryName = Path.GetFileName(childDirectory);
                if (directoryName.Contains("nikon", StringComparison.OrdinalIgnoreCase)
                    || directoryName.Contains("sdk", StringComparison.OrdinalIgnoreCase))
                {
                    yield return childDirectory;
                }
            }
        }

        private static string? FindSdkRoot(string libraryPath)
        {
            DirectoryInfo? directory = new DirectoryInfo(Path.GetDirectoryName(libraryPath)!);
            while (directory is not null)
            {
                if (directory.Name.StartsWith("S-SDK", StringComparison.OrdinalIgnoreCase))
                    return directory.FullName;

                directory = directory.Parent;
            }

            return null;
        }

        private static bool TryFindArchiveModule(string archivePath, out string? moduleEntryPath)
        {
            moduleEntryPath = null;

            try
            {
                using ZipArchive archive = ZipFile.OpenRead(archivePath);
                ZipArchiveEntry? moduleEntry = archive.Entries.FirstOrDefault(entry =>
                    entry.FullName.EndsWith(".md3", StringComparison.OrdinalIgnoreCase)
                    && entry.FullName.Contains("/Module/Win/Binary Files/x64/", StringComparison.OrdinalIgnoreCase));

                if (moduleEntry is null)
                    return false;

                string binaryDirectory = Path.GetDirectoryName(moduleEntry.FullName)!.Replace('\\', '/');
                bool hasTransport = archive.Entries.Any(entry =>
                    string.Equals(entry.FullName.Replace('\\', '/'), $"{binaryDirectory}/NkdPTP.dll", StringComparison.OrdinalIgnoreCase));

                if (!hasTransport)
                    return false;

                moduleEntryPath = moduleEntry.FullName.Replace('\\', '/');
                return true;
            }
            catch
            {
                return false;
            }
        }

        private static int ScoreMaidLocation(NikonMaidSdkLocation location)
        {
            int score = 0;
            string displayName = location.DisplayName;

            if (displayName.Contains("Z9", StringComparison.OrdinalIgnoreCase)
                || displayName.Contains("Z8", StringComparison.OrdinalIgnoreCase)
                || displayName.Contains("Z7", StringComparison.OrdinalIgnoreCase)
                || displayName.Contains("Z6", StringComparison.OrdinalIgnoreCase))
            {
                score += 20;
            }

            if (location.IsArchive)
                score += 5;

            return score;
        }

        private static int ScoreDeviceMatch(NikonMaidSdkLocation location, string normalizedDeviceName)
        {
            if (string.IsNullOrWhiteSpace(normalizedDeviceName))
                return 0;

            string normalizedLocation = NormalizeKey(location.DisplayName);
            if (normalizedLocation.Contains(normalizedDeviceName, StringComparison.Ordinal)
                || normalizedDeviceName.Contains(normalizedLocation, StringComparison.Ordinal))
            {
                return 100;
            }

            foreach (string token in SplitTokens(normalizedLocation))
            {
                if (normalizedDeviceName.Contains(token, StringComparison.Ordinal))
                    return 50;
            }

            return 0;
        }

        private static IEnumerable<string> SplitTokens(string normalizedValue)
        {
            if (string.IsNullOrWhiteSpace(normalizedValue))
                yield break;

            foreach (string token in normalizedValue.Split(new[] { "sdk", "allin", "module" }, StringSplitOptions.RemoveEmptyEntries))
            {
                if (token.Length >= 2)
                    yield return token;
            }
        }

        private static string NormalizeKey(string? value)
        {
            if (string.IsNullOrWhiteSpace(value))
                return string.Empty;

            return new string(value
                .Where(char.IsLetterOrDigit)
                .Select(char.ToLowerInvariant)
                .ToArray());
        }
    }

    public sealed class NikonMaidDevice
    {
        public NikonMaidDevice(string deviceName, string sourcePath)
        {
            DeviceName = deviceName;
            SourcePath = sourcePath;
        }

        public string DeviceName { get; }
        public string SourcePath { get; }
    }
}
