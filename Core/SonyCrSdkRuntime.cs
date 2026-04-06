using System;
using System.Collections.Generic;

namespace OpenStopMotionStudio.Core
{
    public static class SonyCrSdkRuntime
    {
        public static IReadOnlyList<SonySdkDevice> EnumerateConnectedCameras(SonySdkLocation sdkLocation)
        {
            // TODO: Implement P/Invoke calls to CrSDK.dll to enumerate cameras
            return Array.Empty<SonySdkDevice>();
        }
    }
}
