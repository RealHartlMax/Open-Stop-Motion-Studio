namespace OpenStopMotionStudio.Core
{
    public sealed class CameraDeviceDescriptor
    {
        public int Index { get; }
        public int DisplayIndex { get; }
        public string Name { get; }
        public string Vendor { get; }
        public string AdapterName { get; }
        public CameraConnectionKind ConnectionKind { get; }
        public string? ConnectionToken { get; }

        public CameraDeviceDescriptor(
            int index,
            string name,
            string vendor,
            string adapterName,
            CameraConnectionKind connectionKind = CameraConnectionKind.GenericVideo,
            string? connectionToken = null,
            int? displayIndex = null)
        {
            Index = index;
            DisplayIndex = displayIndex ?? index;
            Name = name;
            Vendor = vendor;
            AdapterName = adapterName;
            ConnectionKind = connectionKind;
            ConnectionToken = connectionToken;
        }

        public string DisplayName => string.IsNullOrWhiteSpace(Name)
            ? $"Kamera {DisplayIndex} [{AdapterName}]"
            : $"{DisplayIndex}: {Name} [{AdapterName}]";

        public override string ToString() => DisplayName;
    }
}
