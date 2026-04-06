using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace OpenStopMotionStudio.Core
{
    public sealed class ProjectMigrationService
    {
        public ProjectMigrationReport Migrate(string projectFolder)
        {
            if (string.IsNullOrWhiteSpace(projectFolder) || !Directory.Exists(projectFolder))
                return new ProjectMigrationReport(0, 0);

            int movedFiles = 0;
            int skippedFiles = 0;

            MoveShotDirectoryTree(Path.Combine(projectFolder, "masters"), LegacyProjectPaths.GetMastersRoot(projectFolder), ref movedFiles, ref skippedFiles);
            MoveShotDirectoryTree(Path.Combine(projectFolder, "proxy"), LegacyProjectPaths.GetProxyRoot(projectFolder), ref movedFiles, ref skippedFiles);
            MoveShotDirectoryTree(Path.Combine(projectFolder, "Raw"), LegacyProjectPaths.GetMastersRoot(projectFolder), ref movedFiles, ref skippedFiles);

            MigrateRootFrames(projectFolder, ref movedFiles, ref skippedFiles);
            DeleteEmptyDirectory(Path.Combine(projectFolder, "masters"));
            DeleteEmptyDirectory(Path.Combine(projectFolder, "proxy"));
            DeleteEmptyDirectory(Path.Combine(projectFolder, "Raw"));

            return new ProjectMigrationReport(movedFiles, skippedFiles);
        }

        private static void MigrateRootFrames(string projectFolder, ref int movedFiles, ref int skippedFiles)
        {
            foreach (string filePath in Directory.EnumerateFiles(projectFolder))
            {
                if (!TryParseLegacyFrameName(filePath, out string shotName, out int frameNumber))
                    continue;

                string extension = Path.GetExtension(filePath);
                string targetPath = LegacyProjectPaths.BuildMasterPath(projectFolder, shotName, frameNumber, extension);
                MoveFileSafe(filePath, targetPath, ref movedFiles, ref skippedFiles);
            }
        }

        private static void MoveShotDirectoryTree(string sourceRoot, string targetRoot, ref int movedFiles, ref int skippedFiles)
        {
            if (!Directory.Exists(sourceRoot))
                return;

            foreach (string shotDirectory in Directory.EnumerateDirectories(sourceRoot))
            {
                string shotName = CaptureManager.NormalizeShotName(Path.GetFileName(shotDirectory));
                string normalizedTargetDirectory = Path.Combine(targetRoot, shotName);

                foreach (string filePath in Directory.EnumerateFiles(shotDirectory, "*", SearchOption.AllDirectories))
                {
                    string relativePath = Path.GetRelativePath(shotDirectory, filePath);
                    string targetPath = Path.Combine(normalizedTargetDirectory, relativePath);
                    MoveFileSafe(filePath, targetPath, ref movedFiles, ref skippedFiles);
                }

                DeleteEmptyDirectory(shotDirectory);
            }
        }

        private static void MoveFileSafe(string sourcePath, string targetPath, ref int movedFiles, ref int skippedFiles)
        {
            if (!File.Exists(sourcePath))
                return;

            string? targetDirectory = Path.GetDirectoryName(targetPath);
            if (!string.IsNullOrWhiteSpace(targetDirectory) && !Directory.Exists(targetDirectory))
                Directory.CreateDirectory(targetDirectory);

            if (string.Equals(sourcePath, targetPath, StringComparison.OrdinalIgnoreCase))
                return;

            if (File.Exists(targetPath))
            {
                skippedFiles++;
                return;
            }

            File.Move(sourcePath, targetPath);
            movedFiles++;
        }

        private static void DeleteEmptyDirectory(string path)
        {
            if (!Directory.Exists(path))
                return;

            foreach (string childDirectory in Directory.EnumerateDirectories(path).ToList())
                DeleteEmptyDirectory(childDirectory);

            if (!Directory.EnumerateFileSystemEntries(path).Any())
                Directory.Delete(path);
        }

        private static bool TryParseLegacyFrameName(string path, out string shotName, out int frameNumber)
        {
            shotName = string.Empty;
            frameNumber = 0;

            string fileName = Path.GetFileNameWithoutExtension(path);
            if (string.IsNullOrWhiteSpace(fileName))
                return false;

            int separatorIndex = fileName.LastIndexOf('_');
            if (separatorIndex <= 0 || separatorIndex >= fileName.Length - 1)
                return false;

            if (!int.TryParse(fileName[(separatorIndex + 1)..], out frameNumber) || frameNumber <= 0)
                return false;

            shotName = CaptureManager.NormalizeShotName(fileName[..separatorIndex]);
            return !string.IsNullOrWhiteSpace(shotName);
        }

        private static class LegacyProjectPaths
        {
            public static string GetMastersRoot(string projectFolder) =>
                Path.Combine(projectFolder, "Masters");

            public static string GetProxyRoot(string projectFolder) =>
                Path.Combine(projectFolder, "Proxy");

            public static string GetMastersFolder(string projectFolder, string shotName) =>
                Path.Combine(GetMastersRoot(projectFolder), CaptureManager.NormalizeShotName(shotName));

            public static string BuildMasterPath(string projectFolder, string shotName, int frameIndex, string extension) =>
                Path.Combine(GetMastersFolder(projectFolder, shotName), $"{CaptureManager.NormalizeShotName(shotName)}_{frameIndex:D4}{extension}");
        }
    }
}
