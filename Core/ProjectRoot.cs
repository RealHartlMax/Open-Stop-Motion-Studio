using System;
using System.IO;

namespace OpenStopMotionStudio.Core
{
    public static class ProjectRoot
    {
        private static readonly Lazy<string> _path = new(FindProjectRoot);

        public static string GetPath() => _path.Value;

        private static string FindProjectRoot()
        {
            var currentDir = new DirectoryInfo(AppContext.BaseDirectory);
            while (currentDir != null && !File.Exists(Path.Combine(currentDir.FullName, "Open-Stop-Motion-Studio.sln")))
            {
                currentDir = currentDir.Parent;
            }
            return currentDir?.FullName ?? AppContext.BaseDirectory;
        }
    }
}
