namespace OpenStopMotionStudio.Core
{
    public sealed class SonySdkLocation
    {
        public SonySdkLocation(string sdkRoot, string libraryDirectory, string libraryPath)
        {
            SdkRoot = sdkRoot;
            LibraryDirectory = libraryDirectory;
            LibraryPath = libraryPath;
        }

        public string SdkRoot { get; }
        public string LibraryDirectory { get; }
        public string LibraryPath { get; }
    }
}
