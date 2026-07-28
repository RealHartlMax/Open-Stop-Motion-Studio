using System;
using Avalonia;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using OpenStopMotionStudio.Core;

namespace OpenStopMotionStudio.GUI
{
    public partial class MainWindow
    {
        private DateTime _lastHistogramRefreshUtc = DateTime.MinValue;

        private unsafe void RefreshHistogramPreview(Bitmap liveFrame)
        {
            // Throttle histogram updates to 180ms to avoid excessive processing
            DateTime now = DateTime.UtcNow;
            if ((now - _lastHistogramRefreshUtc).TotalMilliseconds < 180)
                return;

            _lastHistogramRefreshUtc = now;

            if (liveFrame == null)
            {
                ResetHistogramPreview();
                return;
            }

            try
            {
                // Calculate histogram from the live frame
                int[] histogram = CalculateHistogram(liveFrame);
                if (histogram == null || histogram.Length == 0)
                {
                    ResetHistogramPreview(_resourceManager.GetString("HistogramPlaceholder_NoHistogram") ?? "No histogram");
                    return;
                }
                
                // Create and display the histogram bitmap
                Bitmap histogramBitmap = CreateHistogramBitmap(histogram);
                if (histogramBitmap != null && HistogramImage != null)
                {
                    HistogramImage.Source = histogramBitmap;
                }
                
                if (HistogramEmptyText != null)
                    HistogramEmptyText.IsVisible = false;
                
                // Calculate and display statistics
                UpdateHistogramStats(histogram);
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogInfo("HistogramError", $"Error rendering histogram: {ex.Message}");
                ResetHistogramPreview(_resourceManager.GetString("HistogramPlaceholder_RenderError") ?? "Rendering error");
            }
        }

        private unsafe int[] CalculateHistogram(Bitmap bitmap)
        {
            int[] bins = new int[256];
            
            if (bitmap == null || bitmap.PixelSize.Width == 0 || bitmap.PixelSize.Height == 0)
                return bins;

            try
            {
                // Only work with WriteableBitmap which has Lock() method
                if (bitmap is not WriteableBitmap writeableBitmap)
                    return bins;

                using (var framebuffer = writeableBitmap.Lock())
                {
                    if (framebuffer == null || framebuffer.Address == IntPtr.Zero)
                        return bins;

                    // Validate stride
                    int stride = framebuffer.RowBytes;
                    if (stride <= 0 || stride < bitmap.PixelSize.Width * 4)
                    {
                        DebugLogger.Instance.LogInfo("HistogramCalc", $"Invalid stride: {stride} for size {bitmap.PixelSize.Width}x{bitmap.PixelSize.Height}");
                        return bins;
                    }

                    // Sample every 4th pixel to reduce processing time
                    const int sampleFrequency = 4;
                    
                    // Handle BGRA8888 format (standard format used in the app)
                    byte* ptr = (byte*)framebuffer.Address.ToPointer();
                    if (ptr == null)
                        return bins;

                    for (int y = 0; y < bitmap.PixelSize.Height; y += sampleFrequency)
                    {
                        for (int x = 0; x < bitmap.PixelSize.Width; x += sampleFrequency)
                        {
                            // Calculate byte offset: row * stride + column * 4 bytes per pixel
                            int byteOffset = y * stride + x * 4;
                            
                            // Bounds check
                            if (byteOffset + 3 >= stride * bitmap.PixelSize.Height)
                                continue;
                            
                            // BGRA format: B=0, G=1, R=2, A=3
                            byte b = ptr[byteOffset];
                            byte g = ptr[byteOffset + 1];
                            byte r = ptr[byteOffset + 2];
                            
                            // Standard luminance formula
                            int luminance = (int)((r * 0.299) + (g * 0.587) + (b * 0.114));
                            luminance = Math.Min(255, Math.Max(0, luminance));
                            
                            bins[luminance]++;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogInfo("HistogramCalc", $"Error calculating histogram: {ex.Message}");
            }

            return bins;
        }

        private void UpdateHistogramStats(int[] bins)
        {
            try
            {
                if (bins == null || bins.Length == 0)
                    return;

                // Calculate percentage of black (0-25), midtones (100-150), and white (230-255)
                int blackCount = 0, midCount = 0, whiteCount = 0;
                int totalCount = 0;

                for (int i = 0; i < bins.Length; i++)
                {
                    totalCount += bins[i];
                    
                    if (i <= 25)
                        blackCount += bins[i];
                    else if (i >= 100 && i <= 150)
                        midCount += bins[i];
                    else if (i >= 230)
                        whiteCount += bins[i];
                }

                double blackPercent = totalCount > 0 ? (blackCount * 100.0) / totalCount : 0;
                double midPercent = totalCount > 0 ? (midCount * 100.0) / totalCount : 0;
                double whitePercent = totalCount > 0 ? (whiteCount * 100.0) / totalCount : 0;

                if (HistogramStatsText != null)
                {
                    HistogramStatsText.Text = string.Format(_resourceManager.GetString("HistogramStats_Format") ?? "Black {0:F1}% | Mid {1:F1}% | White {2:F1}%", blackPercent, midPercent, whitePercent);
                }
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[Histogram] Error updating stats: {ex.Message}");
            }
        }

        private static unsafe Bitmap CreateHistogramBitmap(int[] bins)
        {
            const int width = 256;
            const int height = 68;
            
            try
            {
                var bitmap = new WriteableBitmap(new PixelSize(width, height), new Vector(96, 96), PixelFormat.Bgra8888, AlphaFormat.Unpremul);

                if (bitmap == null)
                    throw new InvalidOperationException("Failed to create histogram bitmap");

                using (var framebuffer = bitmap.Lock())
                {
                    if (framebuffer == null || framebuffer.Address == IntPtr.Zero)
                        throw new InvalidOperationException("Failed to lock histogram bitmap");

                    uint* ptr = (uint*)framebuffer.Address.ToPointer();
                    if (ptr == null)
                        throw new InvalidOperationException("Histogram buffer pointer is null");

                    int stride = framebuffer.RowBytes / 4;
                    if (stride == 0)
                        stride = width; // Fallback if stride calculation fails

                    // Background
                    for (int i = 0; i < width * height; i++)
                    {
                        ptr[i] = 0xFF14120F; // ABGR format -> 0xFF (A), 14 (B), 12 (G), 0F (R)
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
                        uint color = 0xFF000000 | ((uint)tone << 16) | ((uint)tone << 8) | (uint)tone;

                        for (int y = chartBottom; y > chartBottom - barHeight && y >= 0; y--)
                        {
                            int idx = y * stride + x;
                            if (idx < width * height) // Bounds check
                                ptr[idx] = color;
                        }

                        int baselineIdx = chartBottom * stride + x;
                        if (baselineIdx < width * height) // Bounds check
                            ptr[baselineIdx] = 0xFF606060; // Baseline
                    }
                }
                return bitmap;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[Histogram] Error creating bitmap: {ex.Message}");
                // Return minimal valid bitmap on error
                return new WriteableBitmap(new PixelSize(1, 1), new Vector(96, 96), PixelFormat.Bgra8888, AlphaFormat.Unpremul);
            }
        }

        private void ResetHistogramPreview(string placeholder = "")
        {
            try
            {
                if (HistogramImage != null)
                    HistogramImage.Source = null;
                    
                if (HistogramEmptyText != null)
                {
                    HistogramEmptyText.Text = string.IsNullOrWhiteSpace(placeholder)
                        ? _resourceManager.GetString("HistogramPlaceholder_NoLiveImage") ?? "No live image"
                        : placeholder;
                    HistogramEmptyText.IsVisible = true;
                }
                
                if (HistogramStatsText != null)
                    HistogramStatsText.Text = _resourceManager.GetString("HistogramStats_Default") ?? "Black 0.0% | Mid 0.0% | White 0.0%";
                    
                _lastHistogramRefreshUtc = DateTime.MinValue;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[Histogram] Error resetting preview: {ex.Message}");
            }
        }
    }
}
