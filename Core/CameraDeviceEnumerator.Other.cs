using System.Collections.Generic;

namespace OpenStopMotionStudio.Core
{
    public static partial class CameraDeviceEnumerator
    {
        private static partial List<CameraDeviceDescriptor> EnumerateWindowsDeviceDescriptors(int maxDevices)
        {
            // Non-Windows fallback when the Windows-specific DirectShow implementation is not compiled.
            return new List<CameraDeviceDescriptor>();
        }
    }
}
