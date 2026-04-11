using System;
using System.Collections.Generic;
using System.Globalization;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Controls.Shapes;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Media;
using OpenStopMotionStudio.Core;

namespace OpenStopMotionStudio.GUI
{
    public partial class MainWindow
    {
        private const int DefaultPlaybackFps = 12;
        private const int MaxPlaybackFps = 120;
        private const int FastScrubStep = 5;
        private const int MinTimelineFrames = 250;
        private const double TimelinePixelsPerFrame = 12.0;
        private const double TimelinePadding = 12.0;
        private const double TimelineHeaderHeight = 28.0;
        private const double TimelineLaneHeight = 34.0;
        private const int TimelineLaneCount = 4;

        private int _playbackFps = DefaultPlaybackFps;
        private bool _suppressTimelineFrameScrollBar;
        private int _timelineCursorFrame = 1;
        private bool _isTimelinePointerScrubbing;

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

        private void TimelineFrameScrollBar_ValueChanged(object? sender, RangeBaseValueChangedEventArgs e)
        {
            if (!_uiReady || _suppressTimelineFrameScrollBar)
                return;

            StopPlaybackInternal();
            MoveTimelineCursorToFrame((int)Math.Round(e.NewValue), false);
        }

        private void TimelineScrollViewer_PointerWheelChanged(object? sender, PointerWheelEventArgs e)
        {
            if (!_uiReady)
                return;

            if (e.Handled)
            {
                DebugLogger.Instance.LogInfo("MouseWheel", "Ignored already-handled wheel event (possible duplicate bubble). ");
                return;
            }

            int oldFrame = _timelineCursorFrame;
            DebugLogger.Instance.LogMouseWheel(e.Delta.Y);
            bool fastScrub = (e.KeyModifiers & KeyModifiers.Shift) == KeyModifiers.Shift;
            int scrubStep = fastScrub ? FastScrubStep : 1;
            
            StopPlaybackInternal();
            
            // Mouse wheel up = next frame, down = previous frame.
            // Hold Shift for accelerated scrubbing.
            if (e.Delta.Y > 0)
            {
                int nextFrame = Math.Min(_timelineCursorFrame + scrubStep, GetTimelineEndFrame());
                DebugLogger.Instance.LogFrameNavigation(oldFrame, nextFrame, "Mouse Wheel Up");
                MoveTimelineCursorToFrame(nextFrame, false);
            }
            else if (e.Delta.Y < 0)
            {
                int nextFrame = Math.Max(_timelineCursorFrame - scrubStep, 1);
                DebugLogger.Instance.LogFrameNavigation(oldFrame, nextFrame, "Mouse Wheel Down");
                MoveTimelineCursorToFrame(nextFrame, false);
            }
            
            // Explicitly prevent vertical scrolling by keeping Y-offset at 0
            if (TimelineScrollViewer.Offset.Y != 0)
            {
                TimelineScrollViewer.Offset = new Vector(TimelineScrollViewer.Offset.X, 0);
                DebugLogger.Instance.LogInfo("MouseWheel", "Reset vertical scroll to prevent unwanted vertical movement");
            }
            
            // Ensure focus is on timeline to receive keyboard input
            if (!TimelineScrollViewer.IsFocused)
                TimelineScrollViewer.Focus();
            
            e.Handled = true;
        }

        private void TimelineCanvas_PointerWheelChanged(object? sender, PointerWheelEventArgs e)
        {
            // Delegate to ScrollViewer handler - this intercepts at Canvas level before event bubbles to ScrollViewer
            TimelineScrollViewer_PointerWheelChanged(sender, e);
        }

        private void MoveTimelineCursorToFrame(int frameNumber, bool announce)
        {
            int oldFrame = _timelineCursorFrame;
            _timelineCursorFrame = Math.Clamp(frameNumber, 1, GetTimelineEndFrame());
            
            DebugLogger.Instance.LogTimelineCursorMove(_timelineCursorFrame);

            if (TryGetCapturedFrameByFrameNumber(_timelineCursorFrame, out int captureIndex, out CapturedFrame? frame))
            {
                CapturedFrame selectedFrame = frame!;
                _playbackIndex = captureIndex;
                ShowPlaybackFrame(selectedFrame);
                PlaybackStatusText.Text = $"Keyframe: Frame {selectedFrame.Index} ({captureIndex + 1}/{_capture.Frames.Count})";
                DebugLogger.Instance.LogInfo("Timeline", $"Found keyframe at index {captureIndex}");
            }
            else
            {
                _playbackIndex = -1;
                HidePlaybackPreview();
                PlaybackStatusText.Text = $"Cursor: Frame {_timelineCursorFrame} ohne Capture-Keyframe";
                DebugLogger.Instance.LogInfo("Timeline", $"No keyframe at frame {_timelineCursorFrame}");
            }

            RefreshTimelineState();
            EnsureTimelineCursorVisible();
            RefreshOnionSkinPreview();
            RefreshReferenceOverlayPreview();

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
                // NOTE: Do NOT reset _timelineCursorFrame here - it breaks navigation before first capture!
                // The cursor position should persist independently of frame count.
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
            DrawCapturedMarkers(0, "#E94560", "#FFD1D8");
            DrawCapturedMarkers(1, "#80B8FF", "#F3F6FF");
            DrawTimelinePlayhead(canvasHeight);
        }

        private void DrawTimelineRows(double canvasWidth)
        {
            AddTimelineElement(new Rectangle
            {
                Width = canvasWidth,
                Height = TimelineHeaderHeight,
                Fill = SolidColorBrush.Parse("#1B2034")
            }, 0, 0);

            string[] laneColors =
            {
                "#2C1922",
                "#181E32",
                "#14192B",
                "#111626"
            };

            for (int lane = 0; lane < TimelineLaneCount; lane++)
            {
                AddTimelineElement(new Rectangle
                {
                    Width = canvasWidth,
                    Height = TimelineLaneHeight,
                    Fill = SolidColorBrush.Parse(laneColors[lane])
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
                    StartPoint = new Point(x, 0),
                    EndPoint = new Point(x, canvasHeight),
                    Stroke = SolidColorBrush.Parse(majorLine
                        ? "#4B506B"
                        : mediumLine
                            ? "#343952"
                            : "#24293E"),
                    StrokeThickness = majorLine ? 1.2 : 0.7
                }, 0, 0);

                if (!majorLine)
                    continue;

                var label = new TextBlock
                {
                    Text = frameNumber.ToString(CultureInfo.InvariantCulture),
                    Foreground = SolidColorBrush.Parse("#C9CFE6"),
                    FontSize = 10
                };
                AddTimelineElement(label, x + 3, 5);
            }

            for (int lane = 0; lane <= TimelineLaneCount; lane++)
            {
                double y = TimelineHeaderHeight + lane * TimelineLaneHeight;
                AddTimelineElement(new Line
                {
                    StartPoint = new Point(0, y),
                    EndPoint = new Point(GetTimelineCanvasWidth(timelineEndFrame), y),
                    Stroke = SolidColorBrush.Parse("#2D3244"),
                    StrokeThickness = 1
                }, 0, 0);
            }
        }

        private void DrawCapturedMarkers(int laneIndex, string fillHex, string selectedHex)
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
                    Points = new List<Point>
                    {
                        new Point(x, centerY - 6),
                        new Point(x + 6, centerY),
                        new Point(x, centerY + 6),
                        new Point(x - 6, centerY)
                    },
                    Fill = SolidColorBrush.Parse(isSelected ? selectedHex : fillHex),
                    Stroke = isSelected ? Brushes.White : SolidColorBrush.Parse("#101420"),
                    StrokeThickness = isSelected ? 1.6 : 1.0
                };

                AddTimelineElement(marker, 0, 0);
            }
        }

        private void DrawTimelinePlayhead(double canvasHeight)
        {
            double x = GetTimelineX(_timelineCursorFrame);
            var accentBrush = SolidColorBrush.Parse("#66A3FF");

            AddTimelineElement(new Line
            {
                StartPoint = new Point(x, 0),
                EndPoint = new Point(x, canvasHeight),
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
                    FontWeight = FontWeight.Bold,
                    FontSize = 10
                }
            };

            AddTimelineElement(frameLabel, Math.Max(0, x - 14), 2);
        }

        private void AddTimelineElement(Control element, double left, double top)
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
            if (TimelineScrollViewer.Viewport.Width <= 0)
                return;

            double x = GetTimelineX(_timelineCursorFrame);
            double leftEdge = TimelineScrollViewer.Offset.X;
            double rightEdge = leftEdge + TimelineScrollViewer.Viewport.Width;
            const double margin = 48;

            if (x < leftEdge + margin)
            {
                TimelineScrollViewer.Offset = new Vector(Math.Max(0, x - margin), TimelineScrollViewer.Offset.Y);
            }
            else if (x > rightEdge - margin)
            {
                TimelineScrollViewer.Offset = new Vector(Math.Max(0, x - TimelineScrollViewer.Viewport.Width + margin), TimelineScrollViewer.Offset.Y);
            }
        }

        private int GetTimelineFrameForCanvasX(double canvasX)
        {
            double framePosition = ((canvasX - TimelinePadding) / TimelinePixelsPerFrame) + 1;
            int frameNumber = (int)Math.Round(framePosition, MidpointRounding.AwayFromZero);
            return Math.Clamp(frameNumber, 1, GetTimelineEndFrame());
        }

        private void UpdateTimelineFromPointerPosition(Point position, bool announce)
        {
            StopPlaybackInternal();
            MoveTimelineCursorToFrame(GetTimelineFrameForCanvasX(position.X), announce);
        }
    }
}
