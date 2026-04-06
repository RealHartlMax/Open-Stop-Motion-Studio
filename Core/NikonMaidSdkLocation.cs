namespace OpenStopMotionStudio.Core
{
    public sealed class NikonMaidSdkLocation
    {
        public NikonMaidSdkLocation(string sourcePath, bool isArchive, string moduleRelativePath)
        {
            SourcePath = sourcePath;
            IsArchive = isArchive;
            ModuleRelativePath = moduleRelativePath.Replace('\\', '/');
        }

        public string SourcePath { get; }
        public bool IsArchive { get; }
        public string ModuleRelativePath { get; }
        public string BinaryDirectoryRelativePath => System.IO.Path.GetDirectoryName(ModuleRelativePath)!.Replace('\\', '/');
        public string DisplayName => System.IO.Path.GetFileNameWithoutExtension(SourcePath);
    }
}
