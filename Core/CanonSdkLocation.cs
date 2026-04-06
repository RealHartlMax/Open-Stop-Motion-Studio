namespace OpenStopMotionStudio.Core
{
    public sealed class CanonSdkLocation
    {
        public CanonSdkLocation(string sdkRoot, string libraryDirectory, string libraryPath, string imageLibraryPath)
        {
            SdkRoot = sdkRoot;
            LibraryDirectory = libraryDirectory;
            LibraryPath = libraryPath;
            ImageLibraryPath = imageLibraryPath;
        }

        public string SdkRoot { get; }
        public string LibraryDirectory { get; }
        public string LibraryPath { get; }
        public string ImageLibraryPath { get; }

        public string DisplayName => $"{SdkRoot} ({LibraryDirectory})";
    }
}
