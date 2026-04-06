using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text.Json;
using Emgu.CV;

namespace OpenStopMotionStudio.Core
{
    public static partial class CameraDeviceEnumerator
    {
        private static partial List<CameraDeviceDescriptor> EnumerateMacDeviceDescriptors(int maxDevices)
        {
            var cameraNames = new List<string>();

            try
            {
                var psi = new ProcessStartInfo
                {
                    FileName = "system_profiler",
                    Arguments = "SPCameraDataType -json",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                };

                using var process = Process.Start(psi);
                if (process == null)
                    return new List<CameraDeviceDescriptor>();

                string output = process.StandardOutput.ReadToEnd();
                process.WaitForExit(3000);

                if (process.ExitCode != 0)
                    return new List<CameraDeviceDescriptor>();

                using var document = JsonDocument.Parse(output);
                if (document.RootElement.TryGetProperty("SPCameraDataType", out var cameraArray) &&
                    cameraArray.ValueKind == JsonValueKind.Array)
                {
                    foreach (var camera in cameraArray.EnumerateArray())
                    {
                        if (cameraNames.Count >= maxDevices)
                            break;

                        if (camera.TryGetProperty("_name", out var nameProperty) && nameProperty.ValueKind == JsonValueKind.String)
                        {
                            string name = nameProperty.GetString() ?? string.Empty;
                            if (!string.IsNullOrWhiteSpace(name))
                                cameraNames.Add(name.Trim());
                        }
                    }
                }
            }
            catch
            {
                return new List<CameraDeviceDescriptor>();
            }

            var descriptors = new List<CameraDeviceDescriptor>();
            List<int> workingIndices = Enumerable.Range(0, maxDevices)
                .Where(index =>
                {
                    if (!TryOpenCaptureIndex(index, out VideoCapture capture, VideoCapture.API.Any))
                        return false;

                    using (capture)
                    {
                        return TryReadTestFrame(capture);
                    }
                })
                .ToList();

            for (int order = 0; order < workingIndices.Count; order++)
            {
                int captureIndex = workingIndices[order];
                string name = order < cameraNames.Count ? cameraNames[order] : $"Kamera {captureIndex}";
                string vendor = GetVendorName(name);
                string adapterName = GetAdapterNameForVendor(vendor, name);
                descriptors.Add(new CameraDeviceDescriptor(captureIndex, name, vendor, adapterName));
            }

            return descriptors;
        }

    }
}
