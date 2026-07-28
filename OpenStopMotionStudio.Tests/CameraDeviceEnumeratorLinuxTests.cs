using OpenStopMotionStudio.Core;
using Xunit;

namespace OpenStopMotionStudio.Tests;

public class CameraDeviceEnumeratorLinuxTests
{
    [Fact]
    public void ParseGPhotoAutoDetectOutput_RecognizesCanonCamera()
    {
        const string output = """
            Model                          Port
            Canon EOS 700D (PTP mode)     usb:001,004
            """;

        var descriptors = CameraDeviceEnumerator.ParseGPhotoAutoDetectOutput(output, maxDevices: 8);

        Assert.Single(descriptors);
        Assert.Equal("Canon EOS 700D (PTP mode)", descriptors[0].Name);
        Assert.Equal("Canon", descriptors[0].Vendor);
        Assert.Equal("gphoto2 Adapter", descriptors[0].AdapterName);
        Assert.Equal(CameraConnectionKind.GenericVideo, descriptors[0].ConnectionKind);
        Assert.Equal("usb:001,004", descriptors[0].ConnectionToken?.Trim());
    }

    [Fact]
    public void ParseGPhotoAutoDetectOutput_IgnoresHeaderAndEmptyEntries()
    {
        const string output = """
            Model                          Port
            ----------------------------------------------------------
            Nikon D750                    usb:001,005

            """;

        var descriptors = CameraDeviceEnumerator.ParseGPhotoAutoDetectOutput(output, maxDevices: 8);

        Assert.Single(descriptors);
        Assert.Equal("Nikon D750", descriptors[0].Name);
        Assert.Equal("Nikon", descriptors[0].Vendor);
        Assert.Equal("gphoto2 Adapter", descriptors[0].AdapterName);
        Assert.Equal("usb:001,005", descriptors[0].ConnectionToken?.Trim());
    }
}
