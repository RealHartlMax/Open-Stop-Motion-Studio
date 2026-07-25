using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using Emgu.CV;

namespace OpenStopMotionStudio.Core
{
    public static partial class CameraDeviceEnumerator
    {
        private static partial List<CameraDeviceDescriptor> EnumerateLinuxDeviceDescriptors(int maxDevices)
        {
            var deviceDescriptors = new List<CameraDeviceDescriptor>();
            const string videoClassPath = "/sys/class/video4linux";

            if (Directory.Exists(videoClassPath))
            {
                var deviceDirectories = Directory.EnumerateDirectories(videoClassPath)
                    .Select(Path.GetFileName)
                    .Where(name => !string.IsNullOrWhiteSpace(name))
                    .OrderBy(name => ParseVideoIndex(name!))
                    .Take(maxDevices);

                foreach (var deviceName in deviceDirectories)
                {
                    if (deviceName == null)
                        continue;

                    var nameFile = Path.Combine(videoClassPath, deviceName, "name");
                    string cameraName = deviceName;

                    try
                    {
                        if (File.Exists(nameFile))
                        {
                            string rawName = File.ReadAllText(nameFile).Trim();
                            if (!string.IsNullOrWhiteSpace(rawName))
                                cameraName = rawName;
                        }
                    }
                    catch
                    {
                        cameraName = deviceName;
                    }

                    int index = ParseVideoIndex(deviceName);
                    if (index == int.MaxValue || index >= maxDevices)
                        continue;

                    if (!TryOpenCaptureIndex(index, out VideoCapture capture, VideoCapture.API.Any))
                        continue;

                    using (capture)
                    {
                        bool frameReady = TryReadTestFrame(capture);
                        if (!frameReady && IsLikelyNonCaptureEndpoint(cameraName, deviceName))
                            continue;
                    }

                    string vendor = GetVendorName(cameraName);
                    string adapterName = GetAdapterNameForVendor(vendor, cameraName);
                    deviceDescriptors.Add(new CameraDeviceDescriptor(index, $"{deviceName}: {cameraName}", vendor, adapterName));
                }
            }

            return deviceDescriptors;
        }

        private static bool IsLikelyNonCaptureEndpoint(string cameraName, string deviceName)
        {
            string probe = $"{cameraName} {deviceName}";

            return probe.Contains("metadata", System.StringComparison.OrdinalIgnoreCase)
                || probe.Contains("codec", System.StringComparison.OrdinalIgnoreCase)
                || probe.Contains("m2m", System.StringComparison.OrdinalIgnoreCase)
                || probe.Contains("vbi", System.StringComparison.OrdinalIgnoreCase)
                || probe.Contains("radio", System.StringComparison.OrdinalIgnoreCase)
                || probe.Contains("loopback", System.StringComparison.OrdinalIgnoreCase);
        }

        private static int ParseVideoIndex(string? deviceName)
        {
            var match = Regex.Match(deviceName ?? string.Empty, "video(\\d+)", RegexOptions.IgnoreCase);
            return match.Success && int.TryParse(match.Groups[1].Value, out int index) ? index : int.MaxValue;
        }

    }
}
