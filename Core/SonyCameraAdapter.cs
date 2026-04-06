namespace OpenStopMotionStudio.Core
{
    public sealed class SonyCameraAdapter : VendorCameraAdapterBase
    {
        public override string Name => "Sony DSLR Adapter";

        public SonyCameraAdapter()
        {
        }

        public SonyCameraAdapter(CameraDeviceDescriptor descriptor)
        {
        }

        public override bool Connect(int deviceIndex)
        {
            bool connected = base.Connect(deviceIndex);
            if (connected)
            {
                RaiseStatusChanged("Sony-Kamera verbunden. Live-View wird über generisches Back-End bereitgestellt.");
            }
            return connected;
        }

        public override void CaptureImage()
        {
            RaiseStatusChanged("Sony Capture: Bild wird aufgenommen.");
            base.CaptureImage();
        }
    }
}
