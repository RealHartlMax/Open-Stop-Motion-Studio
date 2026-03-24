using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace OpenStopMotionStudio.GUI
{
    public partial class MainWindow
    {
        private DateTime _lastHistogramRefreshUtc = DateTime.MinValue;

        private void RefreshHistogramPreview(BitmapSource liveFrame)
        {
            DateTime now = DateTime.UtcNow;
            if ((now - _lastHistogramRefreshUtc).TotalMilliseconds < 180)
                return;

            _lastHistogramRefreshUtc = now;

            BitmapSource workingFrame = liveFrame;
            double scale = Math.Min(1.0, Math.Min(192.0 / workingFrame.PixelWidth, 108.0 / workingFrame.PixelHeight));
            if (scale < 1.0)
                workingFrame = new TransformedBitmap(workingFrame, new ScaleTransform(scale, scale));

            workingFrame = new FormatConvertedBitmap(workingFrame, PixelFormats.Gray8, null, 0);

            int width = workingFrame.PixelWidth;
            int height = workingFrame.PixelHeight;
            if (width <= 0 || height <= 0)
                return;

            int stride = width;
            var pixels = new byte[stride * height];
            workingFrame.CopyPixels(pixels, stride, 0);

            var bins = new int[256];
            long sum = 0;
            long blackPixels = 0;
            long whitePixels = 0;

            foreach (byte luminance in pixels)
            {
                bins[luminance]++;
                sum += luminance;

                if (luminance <= 15)
                    blackPixels++;
                if (luminance >= 240)
                    whitePixels++;
            }

            double total = pixels.Length;
            double averagePercent = total > 0 ? sum / total / 255.0 * 100.0 : 0;
            double blackPercent = total > 0 ? blackPixels / total * 100.0 : 0;
            double whitePercent = total > 0 ? whitePixels / total * 100.0 : 0;

            HistogramImage.Source = CreateHistogramBitmap(bins);
            HistogramEmptyText.Visibility = Visibility.Collapsed;
            HistogramStatsText.Text =
                $"Schwarz {blackPercent:0.0}% | Mittel {averagePercent:0.0}% | Weiß {whitePercent:0.0}%";
        }

        private static BitmapSource CreateHistogramBitmap(int[] bins)
        {
            const int width = 256;
            const int height = 68;
            int stride = width * 4;
            var pixels = new byte[stride * height];

            for (int i = 0; i < pixels.Length; i += 4)
            {
                pixels[i] = 0x14;
                pixels[i + 1] = 0x12;
                pixels[i + 2] = 0x0F;
                pixels[i + 3] = 0xFF;
            }

            int maxBin = 0;
            for (int i = 0; i < bins.Length; i++)
            {
                if (bins[i] > maxBin)
                    maxBin = bins[i];
            }

            int chartBottom = height - 6;
            for (int x = 0; x < width; x++)
            {
                double normalized = maxBin > 0 ? Math.Sqrt(bins[x]) / Math.Sqrt(maxBin) : 0;
                int barHeight = (int)Math.Round(normalized * (height - 12));
                byte tone = (byte)x;

                for (int y = chartBottom; y > chartBottom - barHeight && y >= 0; y--)
                {
                    int pixelIndex = y * stride + x * 4;
                    pixels[pixelIndex] = tone;
                    pixels[pixelIndex + 1] = tone;
                    pixels[pixelIndex + 2] = tone;
                    pixels[pixelIndex + 3] = 0xFF;
                }

                int baselineIndex = chartBottom * stride + x * 4;
                pixels[baselineIndex] = 0x60;
                pixels[baselineIndex + 1] = 0x60;
                pixels[baselineIndex + 2] = 0x60;
                pixels[baselineIndex + 3] = 0xFF;
            }

            var bitmap = BitmapSource.Create(width, height, 96, 96, PixelFormats.Bgra32, null, pixels, stride);
            bitmap.Freeze();
            return bitmap;
        }

        private void ResetHistogramPreview(string placeholder = "Kein Live-Bild")
        {
            HistogramImage.Source = null;
            HistogramEmptyText.Text = placeholder;
            HistogramEmptyText.Visibility = Visibility.Visible;
            HistogramStatsText.Text = "Schwarz 0.0% | Mittel 0.0% | Weiß 0.0%";
            _lastHistogramRefreshUtc = DateTime.MinValue;
        }
    }
}
