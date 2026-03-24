using System;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using OpenStopMotionStudio.Core;

namespace OpenStopMotionStudio.GUI
{
    public partial class MainWindow
    {
        private const int DefaultPlaybackFps = 12;
        private const int MaxPlaybackFps = 120;
        private const int MinTimelineFrames = 250;
        private const double TimelinePixelsPerFrame = 12.0;
        private const double TimelinePadding = 12.0;
        private const double TimelineHeaderHeight = 28.0;
        private const double TimelineLaneHeight = 34.0;
        private const int TimelineLaneCount = 4;

        private int _playbackFps = DefaultPlaybackFps;
        private bool _suppressTimelineFrameScrollBar;
        private int _timelineCursorFrame = 1;

        private void PlaybackFpsTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (!_uiReady || e.Key != Key.Enter)
                return;

            ApplyPlaybackFpsFromInput(true);
            e.Handled = true;
        }

        private void PlaybackFpsTextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            if (!_uiReady)
                return;

            ApplyPlaybackFpsFromInput();
        }

        private void ApplyPlaybackFpsFromInput(bool announce = false)
        {
            if (!int.TryParse(PlaybackFpsTextBox.Text, NumberStyles.Integer, CultureInfo.InvariantCulture, out int fps))
                fps = _playbackFps;

            _playbackFps = Math.Clamp(fps, 1, MaxPlaybackFps);
            PlaybackFpsTextBox.Text = _playbackFps.ToString(CultureInfo.InvariantCulture);
            _playbackTimer.Interval = TimeSpan.FromMilliseconds(1000.0 / _playbackFps);
            PlaybackSpeedLabel.Text = $"Playback: {_playbackFps} fps";

            if (announce)
                SetStatus($"Playback-Geschwindigkeit: {_playbackFps} fps");
        }

        private void TimelineFrameScrollBar_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (!_uiReady || _suppressTimelineFrameScrollBar)
                return;

            StopPlaybackInternal();
            MoveTimelineCursorToFrame((int)Math.Round(e.NewValue), false);
        }

        private void TimelineScrollViewer_PreviewMouseWheel(object sender, MouseWheelEventArgs e)
        {
            if (!_uiReady)
                return;

            int deltaFrames = Keyboard.Modifiers.HasFlag(ModifierKeys.Shift) ? 10 : 1;
            int direction = e.Delta > 0 ? -1 : 1;

            StopPlaybackInternal();
            MoveTimelineCursorToFrame(_timelineCursorFrame + direction * deltaFrames, true);
            e.Handled = true;
        }

        private void MoveTimelineCursorToFrame(int frameNumber, bool announce)
        {
            _timelineCursorFrame = Math.Clamp(frameNumber, 1, GetTimelineEndFrame());

            if (TryGetCapturedFrameByFrameNumber(_timelineCursorFrame, out int captureIndex, out CapturedFrame? frame))
            {
                CapturedFrame selectedFrame = frame!;
                _playbackIndex = captureIndex;
                ShowPlaybackFrame(selectedFrame);
                PlaybackStatusText.Text = $"Keyframe: Frame {selectedFrame.Index} ({captureIndex + 1}/{_capture.Frames.Count})";
            }
            else
            {
                _playbackIndex = -1;
                HidePlaybackPreview();
                PlaybackStatusText.Text = $"Cursor: Frame {_timelineCursorFrame} ohne Capture-Keyframe";
            }

            RefreshTimelineState();
            EnsureTimelineCursorVisible();

            if (announce)
            {
                SetStatus(_playbackIndex >= 0
                    ? $"Timeline-Cursor auf Capture-Keyframe {_timelineCursorFrame} gesetzt."
                    : $"Timeline-Cursor auf Frame {_timelineCursorFrame} gesetzt.");
            }
        }

        private bool TryGetCapturedFrameByFrameNumber(int frameNumber, out int captureIndex, out CapturedFrame? frame)
        {
            for (int i = 0; i < _capture.Frames.Count; i++)
            {
                if (_capture.Frames[i].Index != frameNumber)
                    continue;

                captureIndex = i;
                frame = _capture.Frames[i];
                return true;
            }

            captureIndex = -1;
            frame = null;
            return false;
        }

        private int GetNearestCaptureIndexAtOrBefore(int frameNumber)
        {
            if (_capture.Frames.Count == 0)
                return -1;

            for (int i = _capture.Frames.Count - 1; i >= 0; i--)
            {
                if (_capture.Frames[i].Index <= frameNumber)
                    return i;
            }

            return 0;
        }

        private int GetNearestCaptureIndexAtOrAfter(int frameNumber)
        {
            if (_capture.Frames.Count == 0)
                return -1;

            for (int i = 0; i < _capture.Frames.Count; i++)
            {
                if (_capture.Frames[i].Index >= frameNumber)
                    return i;
            }

            return _capture.Frames.Count - 1;
        }

        private void RefreshTimelineState()
        {
            bool hasFrames = _capture.Frames.Count > 0;
            PrevFrameButton.IsEnabled = hasFrames;
            NextFrameButton.IsEnabled = hasFrames;
            PlayPauseButton.IsEnabled = hasFrames;

            if (!hasFrames)
            {
                StopPlaybackInternal();
                HidePlaybackPreview();
                _playbackIndex = -1;
                _timelineCursorFrame = 1;
                PlaybackStatusText.Text = "Timeline leer. Cursor auf Frame 1.";
            }
            else if (_playbackIndex >= 0 && _playbackIndex < _capture.Frames.Count)
            {
                CapturedFrame currentFrame = _capture.Frames[_playbackIndex];
                string modeLabel = _playbackTimer.IsEnabled ? "Playback" : "Keyframe";
                PlaybackStatusText.Text = $"{modeLabel}: Frame {currentFrame.Index} ({_playbackIndex + 1}/{_capture.Frames.Count})";
            }
            else
            {
                PlaybackStatusText.Text = $"Cursor: Frame {_timelineCursorFrame} | {_capture.Frames.Count} Capture-Keyframes";
            }

            TimelineRangeText.Text = $"Start 1 | Ende {GetTimelineEndFrame()}";
            SyncTimelineScrollBar();
            RenderTimeline();
        }

        private void SyncTimelineScrollBar()
        {
            _suppressTimelineFrameScrollBar = true;
            TimelineFrameScrollBar.Minimum = 1;
            TimelineFrameScrollBar.Maximum = GetTimelineEndFrame();
            TimelineFrameScrollBar.SmallChange = 1;
            TimelineFrameScrollBar.LargeChange = 12;
            TimelineFrameScrollBar.Value = Math.Clamp(_timelineCursorFrame, 1, GetTimelineEndFrame());
            _suppressTimelineFrameScrollBar = false;
        }

        private void RenderTimeline()
        {
            TimelineCanvas.Children.Clear();

            int timelineEndFrame = GetTimelineEndFrame();
            double canvasWidth = GetTimelineCanvasWidth(timelineEndFrame);
            double canvasHeight = TimelineHeaderHeight + TimelineLaneHeight * TimelineLaneCount;

            TimelineCanvas.Width = canvasWidth;
            TimelineCanvas.Height = canvasHeight;

            DrawTimelineRows(canvasWidth);
            DrawTimelineGrid(timelineEndFrame, canvasHeight);
            DrawCapturedMarkers(0, Color.FromRgb(0xE9, 0x45, 0x60), Color.FromRgb(0xFF, 0xD1, 0xD8));
            DrawCapturedMarkers(1, Color.FromRgb(0x80, 0xB8, 0xFF), Color.FromRgb(0xF3, 0xF6, 0xFF));
            DrawTimelinePlayhead(canvasHeight);
        }

        private void DrawTimelineRows(double canvasWidth)
        {
            AddTimelineElement(new Rectangle
            {
                Width = canvasWidth,
                Height = TimelineHeaderHeight,
                Fill = new SolidColorBrush(Color.FromRgb(0x1B, 0x20, 0x34))
            }, 0, 0);

            Color[] laneColors =
            {
                Color.FromRgb(0x2C, 0x19, 0x22),
                Color.FromRgb(0x18, 0x1E, 0x32),
                Color.FromRgb(0x14, 0x19, 0x2B),
                Color.FromRgb(0x11, 0x16, 0x26)
            };

            for (int lane = 0; lane < TimelineLaneCount; lane++)
            {
                AddTimelineElement(new Rectangle
                {
                    Width = canvasWidth,
                    Height = TimelineLaneHeight,
                    Fill = new SolidColorBrush(laneColors[lane])
                }, 0, TimelineHeaderHeight + lane * TimelineLaneHeight);
            }
        }

        private void DrawTimelineGrid(int timelineEndFrame, double canvasHeight)
        {
            for (int frameNumber = 1; frameNumber <= timelineEndFrame; frameNumber++)
            {
                double x = GetTimelineX(frameNumber);
                bool majorLine = frameNumber == 1 || frameNumber % 10 == 0;
                bool mediumLine = !majorLine && frameNumber % 5 == 0;

                AddTimelineElement(new Line
                {
                    X1 = x,
                    X2 = x,
                    Y1 = 0,
                    Y2 = canvasHeight,
                    Stroke = new SolidColorBrush(majorLine
                        ? Color.FromRgb(0x4B, 0x50, 0x6B)
                        : mediumLine
                            ? Color.FromRgb(0x34, 0x39, 0x52)
                            : Color.FromRgb(0x24, 0x29, 0x3E)),
                    StrokeThickness = majorLine ? 1.2 : 0.7,
                    SnapsToDevicePixels = true
                }, 0, 0);

                if (!majorLine)
                    continue;

                var label = new TextBlock
                {
                    Text = frameNumber.ToString(CultureInfo.InvariantCulture),
                    Foreground = new SolidColorBrush(Color.FromRgb(0xC9, 0xCF, 0xE6)),
                    FontSize = 10
                };
                AddTimelineElement(label, x + 3, 5);
            }

            for (int lane = 0; lane <= TimelineLaneCount; lane++)
            {
                double y = TimelineHeaderHeight + lane * TimelineLaneHeight;
                AddTimelineElement(new Line
                {
                    X1 = 0,
                    X2 = GetTimelineCanvasWidth(timelineEndFrame),
                    Y1 = y,
                    Y2 = y,
                    Stroke = new SolidColorBrush(Color.FromRgb(0x2D, 0x32, 0x44)),
                    StrokeThickness = 1
                }, 0, 0);
            }
        }

        private void DrawCapturedMarkers(int laneIndex, Color fillColor, Color selectedColor)
        {
            double centerY = GetTimelineLaneCenter(laneIndex);
            int selectedFrameNumber = _playbackIndex >= 0 && _playbackIndex < _capture.Frames.Count
                ? _capture.Frames[_playbackIndex].Index
                : -1;

            foreach (CapturedFrame frame in _capture.Frames)
            {
                double x = GetTimelineX(frame.Index);
                bool isSelected = frame.Index == selectedFrameNumber;

                var marker = new Polygon
                {
                    Points = new PointCollection
                    {
                        new Point(x, centerY - 6),
                        new Point(x + 6, centerY),
                        new Point(x, centerY + 6),
                        new Point(x - 6, centerY)
                    },
                    Fill = new SolidColorBrush(isSelected ? selectedColor : fillColor),
                    Stroke = isSelected ? Brushes.White : new SolidColorBrush(Color.FromRgb(0x10, 0x14, 0x20)),
                    StrokeThickness = isSelected ? 1.6 : 1.0
                };

                AddTimelineElement(marker, 0, 0);
            }
        }

        private void DrawTimelinePlayhead(double canvasHeight)
        {
            double x = GetTimelineX(_timelineCursorFrame);
            var accentBrush = new SolidColorBrush(Color.FromRgb(0x66, 0xA3, 0xFF));

            AddTimelineElement(new Line
            {
                X1 = x,
                X2 = x,
                Y1 = 0,
                Y2 = canvasHeight,
                Stroke = accentBrush,
                StrokeThickness = 2
            }, 0, 0);

            var frameLabel = new Border
            {
                Background = accentBrush,
                CornerRadius = new CornerRadius(4),
                Padding = new Thickness(8, 3, 8, 3),
                Child = new TextBlock
                {
                    Text = _timelineCursorFrame.ToString(CultureInfo.InvariantCulture),
                    Foreground = Brushes.White,
                    FontWeight = FontWeights.Bold,
                    FontSize = 10
                }
            };

            AddTimelineElement(frameLabel, Math.Max(0, x - 14), 2);
        }

        private void AddTimelineElement(UIElement element, double left, double top)
        {
            TimelineCanvas.Children.Add(element);
            Canvas.SetLeft(element, left);
            Canvas.SetTop(element, top);
        }

        private int GetTimelineEndFrame()
        {
            return Math.Max(MinTimelineFrames, Math.Max(_capture.LastFrameNumber + 24, _timelineCursorFrame + 24));
        }

        private double GetTimelineCanvasWidth(int timelineEndFrame)
        {
            return TimelinePadding * 2 + timelineEndFrame * TimelinePixelsPerFrame;
        }

        private double GetTimelineX(int frameNumber)
        {
            return TimelinePadding + (frameNumber - 1) * TimelinePixelsPerFrame + TimelinePixelsPerFrame / 2.0;
        }

        private double GetTimelineLaneCenter(int laneIndex)
        {
            return TimelineHeaderHeight + laneIndex * TimelineLaneHeight + TimelineLaneHeight / 2.0;
        }

        private void EnsureTimelineCursorVisible()
        {
            if (TimelineScrollViewer.ViewportWidth <= 0)
                return;

            double x = GetTimelineX(_timelineCursorFrame);
            double leftEdge = TimelineScrollViewer.HorizontalOffset;
            double rightEdge = leftEdge + TimelineScrollViewer.ViewportWidth;
            const double margin = 48;

            if (x < leftEdge + margin)
            {
                TimelineScrollViewer.ScrollToHorizontalOffset(Math.Max(0, x - margin));
            }
            else if (x > rightEdge - margin)
            {
                TimelineScrollViewer.ScrollToHorizontalOffset(Math.Max(0, x - TimelineScrollViewer.ViewportWidth + margin));
            }
        }
    }
}
