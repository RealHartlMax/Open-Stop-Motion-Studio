using System;
using System.Collections.Generic;
using System.Linq;
using Emgu.CV;

namespace OpenStopMotionStudio.Core
{
    public static partial class CameraDeviceEnumerator
    {
        public static List<CameraDeviceDescriptor> EnumerateDeviceDescriptors(int maxDevices = 16)
        {
            if (OperatingSystem.IsWindows())
                return EnumerateWindowsDeviceDescriptors(maxDevices);

            if (OperatingSystem.IsLinux())
                return EnumerateLinuxDeviceDescriptors(maxDevices);

            if (OperatingSystem.IsMacOS())
                return EnumerateMacDeviceDescriptors(maxDevices);

            return EnumerateGenericDeviceDescriptors(maxDevices);
        }

        public static List<string> EnumerateDeviceNames(int maxDevices = 16)
        {
            return EnumerateDeviceDescriptors(maxDevices).Select(d => d.Name).ToList();
        }

        private static partial List<CameraDeviceDescriptor> EnumerateWindowsDeviceDescriptors(int maxDevices);
        private static partial List<CameraDeviceDescriptor> EnumerateLinuxDeviceDescriptors(int maxDevices);
        private static partial List<CameraDeviceDescriptor> EnumerateMacDeviceDescriptors(int maxDevices);

        private static List<CameraDeviceDescriptor> EnumerateGenericDeviceDescriptors(int maxDevices)
        {
            return new List<CameraDeviceDescriptor>();
        }

        internal static string GetVendorName(string deviceName)
        {
            if (string.IsNullOrWhiteSpace(deviceName))
                return "Generic";

            if (deviceName.Contains("Canon", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("EOS", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Rebel", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Powershot", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("IXUS", System.StringComparison.OrdinalIgnoreCase))
            {
                return "Canon";
            }

            if (deviceName.Contains("Nikon", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Coolpix", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Z6", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Z7", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("D3", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("D5", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("D7", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("D8", System.StringComparison.OrdinalIgnoreCase))
            {
                return "Nikon";
            }

            if (deviceName.Contains("Sony", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("Alpha", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("ILCE", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("ILCA", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("A7", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("A6", System.StringComparison.OrdinalIgnoreCase)
                || deviceName.Contains("A9", System.StringComparison.OrdinalIgnoreCase))
            {
                return "Sony";
            }

            return "Generic";
        }

        internal static string GetAdapterNameForVendor(string vendor, string deviceName)
        {
            if (vendor == "Generic")
            {
                return deviceName.Contains("Webcam", System.StringComparison.OrdinalIgnoreCase)
                    || deviceName.Contains("Logitech", System.StringComparison.OrdinalIgnoreCase)
                    || deviceName.Contains("Brio", System.StringComparison.OrdinalIgnoreCase)
                    || deviceName.Contains("C920", System.StringComparison.OrdinalIgnoreCase)
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

        internal static bool TryOpenCaptureIndex(int index, out VideoCapture capture, params VideoCapture.API[] backends)
        {
            capture = null!;

            foreach (VideoCapture.API backend in backends)
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
                    // keep probing remaining backends
                }
            }

            return false;
        }

        internal static bool TryReadTestFrame(VideoCapture capture)
        {
            using var frame = new Mat();
            if (!capture.Read(frame))
                return false;

            return !frame.IsEmpty;
        }
    }
}
