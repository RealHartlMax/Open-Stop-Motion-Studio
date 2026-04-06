namespace OpenStopMotionStudio.Core
{
    public sealed class ProjectMigrationReport
    {
        public ProjectMigrationReport(int movedFiles, int skippedFiles)
        {
            MovedFiles = movedFiles;
            SkippedFiles = skippedFiles;
        }

        public int MovedFiles { get; }
        public int SkippedFiles { get; }
        public bool HasChanges => MovedFiles > 0;
    }
}
