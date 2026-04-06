using System;
using System.IO;

namespace OpenStopMotionStudio.Core
{
    public static class ProjectPaths
    {
        public const string OriginalCaptureCanon = "original_capture_Canon";
        public const string OriginalCaptureNikon = "original_capture_Nikon";
        public const string OriginalCaptureSony = "original_capture_Sony";
        public const string Proxy = "proxy";
        public const string Raw = "Raw";

        public static string GetLogsFolder(string projectFolder) =>
            Path.Combine(projectFolder, "Logs");

        public static string GetShotRoot(string projectFolder, string shotName)
        {
            return Path.Combine(projectFolder, CaptureManager.NormalizeShotName(shotName));
        }

        public static string GetOriginalsFolder(string projectFolder, string shotName, string extension)
        {
            string vendorFolder = GetVendorFolderFromExtension(extension);
            return Path.Combine(GetShotRoot(projectFolder, shotName), vendorFolder);
        }

        public static string GetProxyFolder(string projectFolder, string shotName)
        {
            return Path.Combine(GetShotRoot(projectFolder, shotName), Proxy);
        }
        
        public static string GetRawFolder(string projectFolder, string shotName)
        {
            return Path.Combine(GetShotRoot(projectFolder, shotName), Raw);
        }

        public static string BuildOriginalPath(string projectFolder, string shotName, int frameIndex, string extension)
        {
            string folder = GetOriginalsFolder(projectFolder, shotName, extension);
            return Path.Combine(folder, $"{CaptureManager.NormalizeShotName(shotName)}_{frameIndex:D5}{extension}");
        }

        public static string BuildProxyPath(string projectFolder, string shotName, int frameIndex)
        {
            string folder = GetProxyFolder(projectFolder, shotName);
            return Path.Combine(folder, $"{CaptureManager.NormalizeShotName(shotName)}_{frameIndex:D5}.jpg");
        }
        
        public static string BuildRawPath(string projectFolder, string shotName, int frameIndex, string extension)
        {
            string folder = GetRawFolder(projectFolder, shotName);
            return Path.Combine(folder, $"{CaptureManager.NormalizeShotName(shotName)}_{frameIndex:D5}{extension}");
        }

        private static string GetVendorFolderFromExtension(string extension)
        {
            return extension.ToLowerInvariant() switch
            {
                ".cr2" or ".cr3" => OriginalCaptureCanon,
                ".nef" or ".nrw" => OriginalCaptureNikon,
                ".arw" or ".srf" or ".sr2" => OriginalCaptureSony,
                _ => "original_capture_other" // Fallback for other cameras
            };
        }
    }
}
