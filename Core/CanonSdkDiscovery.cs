using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace OpenStopMotionStudio.Core
{
    public sealed class CanonSdkDiscovery
    {
        public CanonSdkLocation? FindSdk()
        {
            if (!OperatingSystem.IsWindows())
                return null;

            List<CanonSdkLocation> candidates = new();

            foreach (string root in EnumerateCandidateRoots().Distinct(StringComparer.OrdinalIgnoreCase))
            {
                string canonRoot = Path.Combine(root, "SDKs", "Canon");
                if (!Directory.Exists(canonRoot))
                    continue;

                try
                {
                    foreach (string libraryPath in Directory.EnumerateFiles(canonRoot, "EDSDK.dll", SearchOption.AllDirectories))
                    {
                        string? libraryDirectory = Path.GetDirectoryName(libraryPath);
                        if (string.IsNullOrWhiteSpace(libraryDirectory))
                            continue;

                        string imageLibraryPath = Path.Combine(libraryDirectory, "EdsImage.dll");
                        if (!File.Exists(imageLibraryPath))
                            continue;

                        if (!libraryDirectory.Contains($"{Path.DirectorySeparatorChar}Windows{Path.DirectorySeparatorChar}", StringComparison.OrdinalIgnoreCase))
                            continue;

                        if (!libraryDirectory.Contains("EDSDK_64", StringComparison.OrdinalIgnoreCase)
                            && !libraryDirectory.Contains($"{Path.DirectorySeparatorChar}Dll", StringComparison.OrdinalIgnoreCase))
                        {
                            continue;
                        }

                        string sdkRoot = FindSdkRoot(libraryPath) ?? libraryDirectory;
                        candidates.Add(new CanonSdkLocation(sdkRoot, libraryDirectory, libraryPath, imageLibraryPath));
                    }
                }
                catch (Exception ex)
                {
                    DebugLogger.Instance.LogInfo("CanonSdkDiscovery", $"Failed to scan for Canon SDKs in '{canonRoot}': {ex.Message}");
                }
            }

            return candidates
                .OrderByDescending(location => location.LibraryDirectory.Contains("EDSDK_64", StringComparison.OrdinalIgnoreCase))
                .ThenBy(location => location.LibraryPath, StringComparer.OrdinalIgnoreCase)
                .FirstOrDefault();
        }

        public IReadOnlyList<CanonSdkDevice> EnumerateConnectedCameras()
        {
            CanonSdkLocation? sdkLocation = FindSdk();
            if (sdkLocation == null)
                return Array.Empty<CanonSdkDevice>();

            return CanonEdsdkRuntime.EnumerateConnectedCameras(sdkLocation);
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
                if (directory.Name.StartsWith("EDSDK", StringComparison.OrdinalIgnoreCase))
                    return directory.FullName;

                directory = directory.Parent;
            }

            return null;
        }
    }

    public sealed class CanonSdkDevice
    {
        public CanonSdkDevice(string deviceName, string portName)
        {
            DeviceName = deviceName;
            PortName = portName;
        }

        public string DeviceName { get; }
        public string PortName { get; }
    }
}
