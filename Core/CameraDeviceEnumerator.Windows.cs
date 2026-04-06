using System.Collections.Generic;
using System.Text.RegularExpressions;
using DirectShowLib;
using Emgu.CV;

namespace OpenStopMotionStudio.Core
{
    public static partial class CameraDeviceEnumerator
    {
        private static partial List<CameraDeviceDescriptor> EnumerateWindowsDeviceDescriptors(int maxDevices)
        {
            var deviceDescriptors = new List<CameraDeviceDescriptor>();
            List<WindowsVideoDeviceCandidate> windowsCandidates = EnumerateWindowsVideoDeviceCandidates(maxDevices);
            List<int> availableCaptureIndices = ProbeCaptureIndices(maxDevices, windowsCandidates.Count);

            if (availableCaptureIndices.Count > 0)
            {
                for (int order = 0; order < availableCaptureIndices.Count; order++)
                {
                    int captureIndex = availableCaptureIndices[order];
                    string name = order < windowsCandidates.Count
                        ? windowsCandidates[order].Name
                        : $"Kamera {captureIndex}";

                    string vendor = GetVendorName(name);
                    string adapterName = GetAdapterNameForVendor(vendor, name);
                    deviceDescriptors.Add(new CameraDeviceDescriptor(captureIndex, name, vendor, adapterName));
                }

                return deviceDescriptors;
            }

            foreach (var candidate in windowsCandidates)
            {
                string vendor = GetVendorName(candidate.Name);
                string adapterName = GetAdapterNameForVendor(vendor, candidate.Name);
                deviceDescriptors.Add(new CameraDeviceDescriptor(deviceDescriptors.Count, candidate.Name, vendor, adapterName));
            }

            return deviceDescriptors;
        }

        private static List<WindowsVideoDeviceCandidate> EnumerateWindowsVideoDeviceCandidates(int maxDevices)
        {
            var candidates = new List<WindowsVideoDeviceCandidate>();
            var seenKeys = new HashSet<string>(System.StringComparer.OrdinalIgnoreCase);

            try
            {
                foreach (var device in DsDevice.GetDevicesOfCat(FilterCategory.VideoInputDevice))
                {
                    if (device == null || candidates.Count >= maxDevices)
                        continue;

                    string name = string.IsNullOrWhiteSpace(device.Name)
                        ? "Unbekannte Kamera"
                        : device.Name.Trim();

                    if (!IsLikelyCameraDevice(name))
                        continue;

                    string deduplicationKey = GetDeduplicationKey(device, name);
                    if (!seenKeys.Add(deduplicationKey))
                        continue;

                    candidates.Add(new WindowsVideoDeviceCandidate(name));
                }
            }
            catch
            {
                // ignore and fall back to probe-based listing
            }

            return candidates;
        }

        private static List<int> ProbeCaptureIndices(int maxDevices, int expectedDeviceCount)
        {
            var indices = new List<int>();
            if (expectedDeviceCount <= 0)
                return indices;

            for (int index = 0; index < maxDevices; index++)
            {
                try
                {
                    if (!TryOpenCapture(index, out var capture))
                        continue;

                    using (capture)
                    {
                        if (TryReadTestFrame(capture))
                            indices.Add(index);
                    }

                    if (indices.Count >= expectedDeviceCount)
                        break;
                }
                catch
                {
                    // skip unavailable indices
                }
            }

            return indices;
        }

        private static bool TryOpenCapture(int index, out VideoCapture capture)
        {
            capture = null!;

            VideoCapture.API[] backends = new[] { VideoCapture.API.Msmf, VideoCapture.API.Any };

            foreach (var backend in backends)
            {
                try
                {
                    var tempCapture = new VideoCapture(index, backend);
                    if (tempCapture.IsOpened)
                    {
                        capture = tempCapture;
                        return true;
                    }

                    tempCapture.Dispose();
                }
                catch
                {
                    // ignore backend-specific failures
                }
            }

            return false;
        }

        private static bool IsLikelyCameraDevice(string name)
        {
            if (string.IsNullOrWhiteSpace(name))
                return false;

            string lowerName = name.ToLowerInvariant();
            string[] excludedTerms = new[]
            {
                "screen",
                "display",
                "virtual",
                "loopback",
                "desktop",
                "audio",
                "microphone",
                "obs",
                "manycam",
                "snap",
                "broadcast",
                "screen capture",
                "screen share",
                "v4l2",
                "camera adapter"
            };

            foreach (var term in excludedTerms)
            {
                if (lowerName.Contains(term))
                    return false;
            }

            return true;
        }

        private static string GetDeduplicationKey(DsDevice device, string fallbackName)
        {
            string devicePath = device.DevicePath ?? string.Empty;
            if (string.IsNullOrWhiteSpace(devicePath))
                return NormalizeFallbackName(fallbackName);

            string normalized = devicePath.Trim().ToLowerInvariant();
            normalized = normalized.Replace("@device:pnp:", string.Empty);
            normalized = normalized.Replace("\\global", string.Empty);
            normalized = Regex.Replace(normalized, @"#\{[0-9a-f\-]+\}.*$", string.Empty, RegexOptions.IgnoreCase);
            normalized = Regex.Replace(normalized, @"&mi_[0-9a-f]{2}", string.Empty, RegexOptions.IgnoreCase);
            normalized = Regex.Replace(normalized, @"&col\d+", string.Empty, RegexOptions.IgnoreCase);
            return normalized;
        }

        private static string NormalizeFallbackName(string name)
        {
            return Regex.Replace(name.Trim().ToLowerInvariant(), @"\s+", " ");
        }

        private sealed class WindowsVideoDeviceCandidate
        {
            public WindowsVideoDeviceCandidate(string name)
            {
                Name = name;
            }

            public string Name { get; }
        }
    }
}
