using System;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.Core.Startup
{
    public class DeviceEnumerationTask : IStartupTask
    {
        public string Description => "Initializing SDKs and detecting cameras...";

        public Task ExecuteAsync(Action<string> reportStatus)
        {
            // This is a synchronous, long-running operation, so it's wrapped by Task.Run in the service.
            // Here, we just perform the work.
            CameraManager.Instance.RefreshDeviceList();
            return Task.CompletedTask;
        }
    }
}
