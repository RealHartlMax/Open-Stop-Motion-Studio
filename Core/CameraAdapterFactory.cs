using System;
using System.Collections.Generic;
using System.Linq;

namespace OpenStopMotionStudio.Core
{
    public static class CameraAdapterFactory
    {
        public static ICameraAdapter CreateAdapter(CameraDeviceDescriptor descriptor)
        {
            return descriptor.ConnectionKind switch
            {
                CameraConnectionKind.CanonEdsdk => new CanonCameraAdapter(descriptor),
                CameraConnectionKind.NikonMaid => new NikonCameraAdapter(descriptor),
                CameraConnectionKind.SonyCr => new SonyCameraAdapter(descriptor),
                _ => descriptor.Vendor switch
                {
                    "Canon" => new CanonCameraAdapter(descriptor),
                    "Nikon" => new NikonCameraAdapter(descriptor),
                    "Sony" => new SonyCameraAdapter(),
                    _ => new DirectShowCameraAdapter()
                }
            };
        }

        public static List<CameraDeviceDescriptor> EnumerateDevices()
        {
            List<CameraDeviceDescriptor> deviceDescriptors = CameraDeviceEnumerator.EnumerateDeviceDescriptors();

            if (OperatingSystem.IsWindows())
            {
                deviceDescriptors = MergeCanonSdkDevices(deviceDescriptors);
                deviceDescriptors = MergeNikonSdkDevices(deviceDescriptors);
                deviceDescriptors = MergeSonySdkDevices(deviceDescriptors);
            }

            return ReindexDescriptors(deviceDescriptors);
        }

        private static string GetVendorName(string deviceName)
        {
            if (string.IsNullOrWhiteSpace(deviceName))
                return "Generic";

            if (deviceName.Contains("Canon", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("EOS", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Rebel", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Powershot", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("IXUS", StringComparison.OrdinalIgnoreCase))
            {
                return "Canon";
            }

            if (deviceName.Contains("Nikon", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Coolpix", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Z6", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Z7", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("D3", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("D5", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("D7", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("D8", StringComparison.OrdinalIgnoreCase))
            {
                return "Nikon";
            }

            if (deviceName.Contains("Sony", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Alpha", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("ILCE", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("ILCA", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("A7", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("A6", StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("A9", StringComparison.OrdinalIgnoreCase))
            {
                return "Sony";
            }

            return "Generic";
        }

        private static string GetAdapterNameForVendor(string vendor, string deviceName)
        {
            if (vendor == "Generic")
            {
                return deviceName.Contains("Webcam", StringComparison.OrdinalIgnoreCase)
                    || deviceName.Contains("Logitech", StringComparison.OrdinalIgnoreCase)
                    || deviceName.Contains("Brio", StringComparison.OrdinalIgnoreCase)
                    || deviceName.Contains("C920", StringComparison.OrdinalIgnoreCase)
                    ? "Webcam Adapter"
                    : "Generic UVC Adapter";
            }

            return vendor switch
            {
                "Canon" => "Canon DSLR Adapter",
                "Nikon" => "Nikon DSLR Adapter",
                "Sony" => "Sony DSLR Adapter",
                _ => "Generic UVC Adapter"
            };
        }

        private static List<CameraDeviceDescriptor> MergeCanonSdkDevices(List<CameraDeviceDescriptor> deviceDescriptors)
        {
            var sdkDiscovery = new CanonSdkDiscovery();
            IReadOnlyList<CanonSdkDevice> sdkDevices = sdkDiscovery.EnumerateConnectedCameras();
            if (sdkDevices.Count == 0)
                return deviceDescriptors;

            foreach (CanonSdkDevice sdkDevice in sdkDevices)
            {
                int existingIndex = deviceDescriptors.FindIndex(descriptor =>
                    string.Equals(descriptor.Vendor, "Canon", StringComparison.OrdinalIgnoreCase)
                    && NamesLookEquivalent(descriptor.Name, sdkDevice.DeviceName));

                var sdkDescriptor = new CameraDeviceDescriptor(
                    existingIndex >= 0 ? deviceDescriptors[existingIndex].Index : -1,
                    sdkDevice.DeviceName,
                    "Canon",
                    "Canon DSLR Adapter (EDSDK)",
                    CameraConnectionKind.CanonEdsdk,
                    sdkDevice.PortName);

                if (existingIndex >= 0)
                {
                    deviceDescriptors[existingIndex] = sdkDescriptor;
                }
                else
                {
                    deviceDescriptors.Add(sdkDescriptor);
                }
            }

            return deviceDescriptors;
        }

        private static List<CameraDeviceDescriptor> MergeNikonSdkDevices(List<CameraDeviceDescriptor> deviceDescriptors)
        {
            var sdkDiscovery = new NikonSdkDiscovery();
            IReadOnlyList<NikonMaidDevice> sdkDevices = sdkDiscovery.EnumerateConnectedMaidCameras();
            if (sdkDevices.Count == 0)
                return deviceDescriptors;

            foreach (NikonMaidDevice sdkDevice in sdkDevices)
            {
                int existingIndex = deviceDescriptors.FindIndex(descriptor =>
                    string.Equals(descriptor.Vendor, "Nikon", StringComparison.OrdinalIgnoreCase)
                    && NamesLookEquivalent(descriptor.Name, sdkDevice.DeviceName));

                var sdkDescriptor = new CameraDeviceDescriptor(
                    existingIndex >= 0 ? deviceDescriptors[existingIndex].Index : -1,
                    sdkDevice.DeviceName,
                    "Nikon",
                    "Nikon DSLR Adapter (MAID)",
                    CameraConnectionKind.NikonMaid,
                    sdkDevice.SourcePath);

                if (existingIndex >= 0)
                {
                    deviceDescriptors[existingIndex] = sdkDescriptor;
                }
                else
                {
                    deviceDescriptors.Add(sdkDescriptor);
                }
            }

            return deviceDescriptors;
        }

        private static List<CameraDeviceDescriptor> MergeSonySdkDevices(List<CameraDeviceDescriptor> deviceDescriptors)
        {
            var sdkDiscovery = new SonySdkDiscovery();
            IReadOnlyList<SonySdkDevice> sdkDevices = sdkDiscovery.EnumerateConnectedCameras();
            if (sdkDevices.Count == 0)
                return deviceDescriptors;

            foreach (SonySdkDevice sdkDevice in sdkDevices)
            {
                int existingIndex = deviceDescriptors.FindIndex(descriptor =>
                    string.Equals(descriptor.Vendor, "Sony", StringComparison.OrdinalIgnoreCase)
                    && NamesLookEquivalent(descriptor.Name, sdkDevice.DeviceName));

                var sdkDescriptor = new CameraDeviceDescriptor(
                    existingIndex >= 0 ? deviceDescriptors[existingIndex].Index : -1,
                    sdkDevice.DeviceName,
                    "Sony",
                    "Sony DSLR Adapter (CrSDK)",
                    CameraConnectionKind.SonyCr,
                    sdkDevice.PortName);

                if (existingIndex >= 0)
                {
                    deviceDescriptors[existingIndex] = sdkDescriptor;
                }
                else
                {
                    deviceDescriptors.Add(sdkDescriptor);
                }
            }

            return deviceDescriptors;
        }

        private static List<CameraDeviceDescriptor> ReindexDescriptors(List<CameraDeviceDescriptor> deviceDescriptors)
        {
            return deviceDescriptors
                .Select((descriptor, order) => new CameraDeviceDescriptor(
                    descriptor.Index,
                    descriptor.Name,
                    descriptor.Vendor,
                    descriptor.AdapterName,
                    descriptor.ConnectionKind,
                    descriptor.ConnectionToken,
                    order))
                .ToList();
        }

        private static bool NamesLookEquivalent(string left, string right)
        {
            string normalizedLeft = NormalizeDeviceName(left);
            string normalizedRight = NormalizeDeviceName(right);

            if (normalizedLeft.Length == 0 || normalizedRight.Length == 0)
                return false;

            return normalizedLeft.Contains(normalizedRight, StringComparison.Ordinal)
                || normalizedRight.Contains(normalizedLeft, StringComparison.Ordinal);
        }

        private static string NormalizeDeviceName(string value)
        {
            Span<char> buffer = stackalloc char[value.Length];
            int length = 0;

            foreach (char ch in value)
            {
                if (!char.IsLetterOrDigit(ch))
                    continue;

                buffer[length++] = char.ToLowerInvariant(ch);
            }

            return new string(buffer[..length]);
        }
    }
}
