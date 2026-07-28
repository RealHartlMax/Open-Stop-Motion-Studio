using OpenStopMotionStudio.Core;
using Xunit;

namespace OpenStopMotionStudio.Tests;

public class CaptureManagerProjectLoadTests
{
    [Fact]
    public void LoadProjectFramesFromDisk_ReturnsPlaceholderPreview_WhenPreviewImageIsUnreadable()
    {
        string tempRoot = Path.Combine(Path.GetTempPath(), "osms-tests", Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(tempRoot);

        try
        {
            var captureManager = new CaptureManager();
            captureManager.SetOutputFolder(tempRoot);

            string shotDirectory = Path.Combine(tempRoot, "shot1");
            string originalDirectory = Path.Combine(shotDirectory, ProjectPaths.OriginalCaptureCanon);
            Directory.CreateDirectory(originalDirectory);

            string masterPath = Path.Combine(originalDirectory, "shot1_00001.jpg");
            File.WriteAllText(masterPath, "not a valid image");

            var summary = captureManager.LoadProjectFramesFromDisk();

            Assert.NotNull(summary);
            Assert.Equal("shot1", summary!.ShotName);
            Assert.Single(summary.Frames);
            Assert.Null(summary.Frames[0].PreviewFrame);
        }
        finally
        {
            if (Directory.Exists(tempRoot))
            {
                Directory.Delete(tempRoot, recursive: true);
            }
        }
    }
}
