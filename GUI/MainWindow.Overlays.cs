using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Shapes;
using Avalonia.Media;
using OpenStopMotionStudio.Core;
using System;

namespace OpenStopMotionStudio.GUI
{
    public partial class MainWindow
    {
        /// <summary>
        /// Zeichnet die Kompositions-Overlays (Grid, Action Safe, Title Safe) auf das Canvas.
        /// Passt sich dynamisch an die aktuelle Auflösung an.
        /// </summary>
        private void RefreshOverlayCanvas()
        {
            try
            {
                OverlayCanvas.Children.Clear();

                double displayWidth = OverlayCanvas.Bounds.Width;
                double displayHeight = OverlayCanvas.Bounds.Height;

                if (displayWidth <= 0 || displayHeight <= 0)
                    return;

                // Hole die ECHTE Kamera-Auflösung (nicht die Display-Größe!)
                var cameraResolution = _camera.GetCurrentResolution();
                
                // Wenn Kamera nicht läuft oder keine Auflösung, abbrechen
                if (cameraResolution.Width <= 0 || cameraResolution.Height <= 0)
                    return;

                // Berechne die tatsächlich angezeigte Bildfläche innerhalb des Canvas bei Uniform-Stretch.
                double imageAspect = (double)cameraResolution.Width / cameraResolution.Height;
                double containerAspect = displayWidth / displayHeight;

                double imageWidth;
                double imageHeight;
                double imageOffsetX;
                double imageOffsetY;

                if (containerAspect > imageAspect)
                {
                    imageHeight = displayHeight;
                    imageWidth = displayHeight * imageAspect;
                    imageOffsetX = (displayWidth - imageWidth) / 2.0;
                    imageOffsetY = 0;
                }
                else
                {
                    imageWidth = displayWidth;
                    imageHeight = displayWidth / imageAspect;
                    imageOffsetX = 0;
                    imageOffsetY = (displayHeight - imageHeight) / 2.0;
                }

                // Speichere die echte Auflösung im Manager
                _overlay.CurrentResolution = cameraResolution;

                // Grid Lines
                if (_overlay.ShowGrid)
                {
                    DrawGrid(imageWidth, imageHeight, imageOffsetX, imageOffsetY);
                }

                // Action Safe Zone (90% des Bildes)
                if (_overlay.ShowActionSafe)
                {
                    DrawSafeZone(imageWidth, imageHeight, 0.90, "#FFD700", "Action Safe", imageOffsetX, imageOffsetY);
                }

                // Title Safe Zone (80% des Bildes)
                if (_overlay.ShowTitleSafe)
                {
                    DrawSafeZone(imageWidth, imageHeight, 0.80, "#00FF00", "Title Safe", imageOffsetX, imageOffsetY);
                }

                // Zeichne Auflösungs-Info in unten rechts
                DrawResolutionInfo(displayWidth, displayHeight);
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.Log($"[ERROR] RefreshOverlayCanvas: {ex.Message}");
            }
        }

        /// <summary>
        /// Zeichnet die aktuelle Auflösung und das Aspect Ratio als Info-Text.
        /// </summary>
        private void DrawResolutionInfo(double displayWidth, double displayHeight)
        {
            try
            {
                var resolution = _overlay.CurrentResolution;
                if (resolution == null || resolution.Width <= 0 || resolution.Height <= 0)
                    return;

                string infoText = $"{resolution.Width}x{resolution.Height} ({resolution.AspectRatio})";

                var infoBlock = new TextBlock
                {
                    Text = infoText,
                    Foreground = new SolidColorBrush(Color.Parse("#666666")),
                    FontSize = 9,
                    Opacity = 0.4
                };

                // Avalonia nutzt nur SetLeft und SetTop, nicht SetRight/SetBottom
                double posX = Math.Max(0, displayWidth - 120);  // 120px von rechts
                double posY = Math.Max(0, displayHeight - 20);  // 20px von unten

                Canvas.SetLeft(infoBlock, posX);
                Canvas.SetTop(infoBlock, posY);
                OverlayCanvas.Children.Add(infoBlock);
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.Log($"[ERROR] DrawResolutionInfo: {ex.Message}");
            }
        }

        /// <summary>
        /// Zeichnet ein Gitter für die Bildkomposition basierend auf der echten Kamera-Auflösung.
        /// Regel der Drittel: 3x3 Gitter
        /// </summary>
        private void DrawGrid(double imageWidth, double imageHeight, double offsetX, double offsetY)
        {
            try
            {
                double thirdW = imageWidth / 3.0;
                double thirdH = imageHeight / 3.0;

                // Vertikale Linien
                for (int i = 1; i < 3; i++)
                {
                    var line = new Line
                    {
                        StartPoint = new Point(offsetX + thirdW * i, offsetY),
                        EndPoint = new Point(offsetX + thirdW * i, offsetY + imageHeight),
                        Stroke = new SolidColorBrush(Color.Parse("#CCFFFF")),
                        StrokeThickness = 0.5,
                        Opacity = 0.5
                    };
                    OverlayCanvas.Children.Add(line);
                }

                // Horizontale Linien
                for (int i = 1; i < 3; i++)
                {
                    var line = new Line
                    {
                        StartPoint = new Point(offsetX, offsetY + thirdH * i),
                        EndPoint = new Point(offsetX + imageWidth, offsetY + thirdH * i),
                        Stroke = new SolidColorBrush(Color.Parse("#CCFFFF")),
                        StrokeThickness = 0.5,
                        Opacity = 0.5
                    };
                    OverlayCanvas.Children.Add(line);
                }

                double centerX = offsetX + imageWidth / 2.0;
                double centerY = offsetY + imageHeight / 2.0;
                double crossSize = 20;

                var crossH = new Line
                {
                    StartPoint = new Point(centerX - crossSize, centerY),
                    EndPoint = new Point(centerX + crossSize, centerY),
                    Stroke = new SolidColorBrush(Color.Parse("#FFAA00")),
                    StrokeThickness = 1,
                    Opacity = 0.6
                };
                OverlayCanvas.Children.Add(crossH);

                var crossV = new Line
                {
                    StartPoint = new Point(centerX, centerY - crossSize),
                    EndPoint = new Point(centerX, centerY + crossSize),
                    Stroke = new SolidColorBrush(Color.Parse("#FFAA00")),
                    StrokeThickness = 1,
                    Opacity = 0.6
                };
                OverlayCanvas.Children.Add(crossV);
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.Log($"[ERROR] DrawGrid: {ex.Message}");
            }
        }

        /// <summary>
        /// Zeichnet eine Safe Zone (ActionSafe bei 90%, TitleSafe bei 80%) basierend auf dem angezeigten Bildbereich.
        /// </summary>
        private void DrawSafeZone(double imageWidth, double imageHeight, double sizePercent, string colorHex, string label, double offsetX, double offsetY)
        {
            try
            {
                double w = imageWidth * sizePercent;
                double h = imageHeight * sizePercent;
                double x = offsetX + (imageWidth - w) / 2.0;
                double y = offsetY + (imageHeight - h) / 2.0;

                var rect = new Rectangle
                {
                    Width = w,
                    Height = h,
                    Stroke = new SolidColorBrush(Color.Parse(colorHex)),
                    StrokeThickness = 2,
                    Fill = null,
                    Opacity = 0.7
                };

                Canvas.SetLeft(rect, x);
                Canvas.SetTop(rect, y);
                OverlayCanvas.Children.Add(rect);

                var labelBlock = new TextBlock
                {
                    Text = label,
                    Foreground = new SolidColorBrush(Color.Parse(colorHex)),
                    FontSize = 10,
                    Opacity = 0.8
                };

                Canvas.SetLeft(labelBlock, x + 4);
                Canvas.SetTop(labelBlock, y + 2);
                OverlayCanvas.Children.Add(labelBlock);
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.Log($"[ERROR] DrawSafeZone: {ex.Message}");
            }
        }
    }
}
