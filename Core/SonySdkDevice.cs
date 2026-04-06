namespace OpenStopMotionStudio.Core
{
    public sealed class SonySdkDevice
    {
        public SonySdkDevice(string deviceName, string portName)
        {
            DeviceName = deviceName;
            PortName = portName;
        }

        public string DeviceName { get; }
        public string PortName { get; }
    }
}
