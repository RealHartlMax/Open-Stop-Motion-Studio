using System;
using System.Collections.Generic;
using System.IO;
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

            return candidates
                .OrderByDescending(location => location.LibraryDirectory.Contains($"{Path.DirectorySeparatorChar}Release", StringComparison.OrdinalIgnoreCase))
                .ThenByDescending(location => location.SdkRoot.Contains("S-SDKNEF", StringComparison.OrdinalIgnoreCase))
                .ThenBy(location => location.LibraryPath, StringComparer.OrdinalIgnoreCase)
                .FirstOrDefault();
        }

        private static IEnumerable<string> EnumerateCandidateRoots()
        {
            yield return Environment.CurrentDirectory;
            yield return AppContext.BaseDirectory;

            foreach (string seed in new[] { Environment.CurrentDirectory, AppContext.BaseDirectory })
            {
                DirectoryInfo? directory = new DirectoryInfo(seed);
                while (directory is not null)
                {
                    yield return directory.FullName;
                    directory = directory.Parent;
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
    }
}
