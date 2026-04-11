using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace OpenStopMotionStudio.Core
{
    public sealed class SonySdkDiscovery
    {
        public SonySdkLocation? FindSdk()
        {
            if (!OperatingSystem.IsWindows())
                return null;

            List<SonySdkLocation> candidates = new();

            foreach (string root in EnumerateCandidateRoots().Distinct(StringComparer.OrdinalIgnoreCase))
            {
                string sonyRoot = Path.Combine(root, "SDKs", "Sony");
                if (!Directory.Exists(sonyRoot))
                    continue;

                try
                {
                    foreach (string libraryPath in Directory.EnumerateFiles(sonyRoot, "CrSDK.dll", SearchOption.AllDirectories))
                    {
                        string? libraryDirectory = Path.GetDirectoryName(libraryPath);
                        if (string.IsNullOrWhiteSpace(libraryDirectory))
                            continue;

                        if (!libraryDirectory.Contains($"{Path.DirectorySeparatorChar}Windows{Path.DirectorySeparatorChar}", StringComparison.OrdinalIgnoreCase) && !libraryDirectory.Contains("Win64", StringComparison.OrdinalIgnoreCase))
                            continue;

                        string sdkRoot = FindSdkRoot(libraryPath) ?? libraryDirectory;
                        candidates.Add(new SonySdkLocation(sdkRoot, libraryDirectory, libraryPath));
                    }
                }
                catch (Exception ex)
                {
                    DebugLogger.Instance.LogInfo("SonySdkDiscovery", $"Failed to scan for Sony SDKs in '{sonyRoot}': {ex.Message}");
                }
            }

            return candidates
                .OrderByDescending(location => location.LibraryDirectory.Contains("Win64", StringComparison.OrdinalIgnoreCase))
                .ThenBy(location => location.LibraryPath, StringComparer.OrdinalIgnoreCase)
                .FirstOrDefault();
        }

        public IReadOnlyList<SonySdkDevice> EnumerateConnectedCameras()
        {
            SonySdkLocation? sdkLocation = FindSdk();
            if (sdkLocation == null)
                return Array.Empty<SonySdkDevice>();

            try
            {
                return SonyCrSdkRuntime.EnumerateConnectedCameras(sdkLocation);
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogInfo("SonySdkDiscovery", $"Sony CrSDK enumeration failed: {ex.Message}");
                return Array.Empty<SonySdkDevice>();
            }
        }

        private static IEnumerable<string> EnumerateCandidateRoots()
        {
            yield return ProjectRoot.GetPath();
            yield return AppContext.BaseDirectory;
        }

        private static string? FindSdkRoot(string libraryPath)
        {
            DirectoryInfo? directory = new DirectoryInfo(Path.GetDirectoryName(libraryPath)!);
            while (directory is not null)
            {
                if (directory.Name.StartsWith("CrSDK", StringComparison.OrdinalIgnoreCase))
                    return directory.FullName;

                directory = directory.Parent;
            }

            return null;
        }
    }
}
