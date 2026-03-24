namespace OpenStopMotionStudio.Core
{
    public sealed class NikonImageSdkLocation
    {
        public NikonImageSdkLocation(string sdkRoot, string libraryDirectory, string libraryPath)
        {
            SdkRoot = sdkRoot;
            LibraryDirectory = libraryDirectory;
            LibraryPath = libraryPath;
        }

        public string SdkRoot { get; }
        public string LibraryDirectory { get; }
        public string LibraryPath { get; }
        public string DisplayName => System.IO.Path.GetFileName(SdkRoot);
    }
}
