namespace OpenStopMotionStudio.Core
{
    public sealed class NefImportProgress
    {
        public NefImportProgress(int current, int total, string fileName)
        {
            Current = current;
            Total = total;
            FileName = fileName;
        }

        public int Current { get; }
        public int Total { get; }
        public string FileName { get; }
    }
}
