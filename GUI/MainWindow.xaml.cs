using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using Microsoft.Win32;
using OpenStopMotionStudio.Core;

namespace OpenStopMotionStudio.GUI
{
    public partial class MainWindow : Window
    {
        private readonly CameraManager _camera = new();
        private readonly CaptureManager _capture = new();
        private readonly OverlayManager _overlay = new();
        private readonly StreamDeckManager _streamDeck = new();
        private readonly DispatcherTimer _playbackTimer = new();

        private Image[] _onionSkinLayers = Array.Empty<Image>();
        private bool _uiReady;
        private int _playbackIndex = -1;

        public MainWindow()
        {
            InitializeComponent();
            BootstrapApplication();
        }

        private void BootstrapApplication()
        {
            _onionSkinLayers = new[] { OnionSkinImageLayer1, OnionSkinImageLayer2, OnionSkinImageLayer3 };

            _camera.FrameReady += OnFrameReady;

            _streamDeck.CapturePressed += () => Dispatcher.Invoke(TriggerCapture);
            _streamDeck.OnionTogglePressed += () => Dispatcher.Invoke(ToggleOnionSkin);
            _streamDeck.AlphaPresetPressed += value => Dispatcher.Invoke(() => SetAlphaPreset(value));
            _streamDeck.UndoPressed += () => Dispatcher.Invoke(UndoLastCapture);

            _playbackTimer.Tick += PlaybackTimer_Tick;

            RefreshCameraList();

            CaptureFormatComboBox.SelectedIndex = 0;
            OnionLayerComboBox.SelectedIndex = 0;
            _overlay.OnionLayers = 1;
            _overlay.AlphaValue = AlphaSlider.Value / 100.0;

            PlaybackFpsTextBox.Text = DefaultPlaybackFps.ToString();
            ApplyPlaybackFpsFromInput();
            ShotNameTextBox.Text = _capture.ShotName;
            ProjectFolderText.Text = _capture.OutputFolder;
            ResetHistogramPreview();
            UpdateShotPreview();
            InitializeRawImportUi();
            UpdateFrameCounterText();
            UpdateCameraSettingsButtonState();
            RefreshTimelineState();

            _uiReady = true;
            KeyDown += MainWindow_KeyDown;
            SetStatus("Bereit. Kamera auswählen und starten.");
        }

        private void RefreshCameraList()
        {
            CameraComboBox.Items.Clear();
            var devices = _camera.GetAvailableDevices();

            if (devices.Count == 0)
            {
                CameraComboBox.Items.Add("Keine Kamera gefunden");
                StartCameraButton.IsEnabled = false;
            }
            else
            {
                foreach (var device in devices)
                    CameraComboBox.Items.Add(device);

                CameraComboBox.SelectedIndex = 0;
                StartCameraButton.IsEnabled = true;
            }
        }

        private void CameraComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!_uiReady)
                return;

            if (_camera.IsRunning)
            {
                _camera.Stop();
                StartCameraButton.Content = "▶ Kamera starten";
                CaptureButton.IsEnabled = false;
                LiveFeedImage.Source = null;
                NoCameraText.Text = "Kamera geändert.\nZum Fortfahren erneut starten.";
                NoCameraText.Visibility = Visibility.Visible;
                ResetHistogramPreview("Kamera gewechselt");
                SetStatus("Kameraauswahl geändert.");
            }

            StopPlaybackInternal();
            HidePlaybackPreview();
            _playbackIndex = -1;
            UpdateCameraSettingsButtonState();
            RefreshTimelineState();
        }

        private void StartCameraButton_Click(object sender, RoutedEventArgs e)
        {
            StopPlaybackInternal();
            HidePlaybackPreview();
            _playbackIndex = -1;

            if (_camera.IsRunning)
            {
                _camera.Stop();
                StartCameraButton.Content = "▶ Kamera starten";
                CaptureButton.IsEnabled = false;
                LiveFeedImage.Source = null;
                NoCameraText.Text = "Keine Kamera verbunden.\nKamera in der Sidebar auswählen.";
                NoCameraText.Visibility = Visibility.Visible;
                ResetHistogramPreview();
                UpdateCameraSettingsButtonState();
                RefreshTimelineState();
                SetStatus("Kamera gestoppt.");
                return;
            }

            int selectedIndex = CameraComboBox.SelectedIndex;
            if (selectedIndex < 0)
                return;

            bool started = _camera.Start(selectedIndex);
            if (started)
            {
                StartCameraButton.Content = "⏹ Kamera stoppen";
                CaptureButton.IsEnabled = true;
                LiveFeedImage.Source = null;
                NoCameraText.Text = "Verbinde Kamera...\nWarte auf erstes Bild.";
                NoCameraText.Visibility = Visibility.Visible;
                ResetHistogramPreview("Warte auf Live-Bild");
                UpdateCameraSettingsButtonState();
                RefreshTimelineState();
                SetStatus($"Kamera aktiv: {CameraComboBox.SelectedItem}");
            }
            else
            {
                UpdateCameraSettingsButtonState();
                MessageBox.Show(
                    "Kamera konnte nicht gestartet werden.",
                    "Fehler",
                    MessageBoxButton.OK,
                    MessageBoxImage.Warning);
            }
        }

        private void OnFrameReady(BitmapSource liveFrame)
        {
            Dispatcher.Invoke(() =>
            {
                LiveFeedImage.Source = liveFrame;
                NoCameraText.Visibility = Visibility.Collapsed;
                RefreshOnionSkinPreview();
                RefreshHistogramPreview(liveFrame);
            });
        }

        private void CaptureButton_Click(object sender, RoutedEventArgs e) => TriggerCapture();

        private void MainWindow_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Space && CaptureButton.IsEnabled)
                TriggerCapture();
        }

        private void TriggerCapture()
        {
            StopPlaybackInternal();
            HidePlaybackPreview();
            _playbackIndex = -1;

            var currentFrame = _camera.GetCurrentFrame();
            if (currentFrame == null)
            {
                SetStatus("⚠ Kein Frame verfügbar.");
                return;
            }

            CapturedFrame capturedFrame = _capture.SaveFrame(currentFrame);
            _timelineCursorFrame = capturedFrame.Index;
            UpdateFrameCounterText();

            RefreshOnionSkinPreview();
            UpdateShotPreview();
            RefreshTimelineState();
            EnsureTimelineCursorVisible();

            PlaybackStatusText.Text = $"Capture-Keyframe: Frame {capturedFrame.Index} gespeichert";
            SetStatus($"✔ Frame {capturedFrame.Index} gespeichert: {System.IO.Path.GetFileName(capturedFrame.MasterPath)}");
        }

        private void UndoLastCapture()
        {
            if (!_capture.UndoLastCapture())
            {
                SetStatus("Kein Frame zum Rückgängig machen.");
                return;
            }

            StopPlaybackInternal();
            HidePlaybackPreview();
            _playbackIndex = -1;

            _timelineCursorFrame = _capture.Frames.Count > 0 ? _capture.Frames[^1].Index : 1;

            UpdateFrameCounterText();
            RefreshOnionSkinPreview();
            UpdateShotPreview();
            RefreshTimelineState();
            EnsureTimelineCursorVisible();

            SetStatus(_capture.FrameCount > 0
                ? $"Letztes Frame entfernt. Aktuell: {_capture.LastFrameNumber}"
                : "Letztes Frame entfernt. Keine Frames mehr vorhanden.");
        }

        private void OnionSkinToggle_Checked(object sender, RoutedEventArgs e)
        {
            _overlay.IsEnabled = true;
            OnionSkinToggle.Content = "Onion Skin: AN ✓";
            RefreshOnionSkinPreview();
        }

        private void OnionSkinToggle_Unchecked(object sender, RoutedEventArgs e)
        {
            _overlay.IsEnabled = false;
            OnionSkinToggle.Content = "Onion Skin: AUS";
            ClearOnionSkinLayers();
        }

        private void ToggleOnionSkin()
        {
            OnionSkinToggle.IsChecked = !OnionSkinToggle.IsChecked;
        }

        private void OnionLayerComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!_uiReady || OnionLayerComboBox.SelectedIndex < 0)
                return;

            _overlay.OnionLayers = OnionLayerComboBox.SelectedIndex + 1;
            RefreshOnionSkinPreview();
        }

        private void AlphaSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (!_uiReady)
                return;

            double alpha = e.NewValue / 100.0;
            _overlay.AlphaValue = alpha;
            AlphaLabel.Text = $"Transparenz: {(int)e.NewValue}%";
            RefreshOnionSkinPreview();
        }

        private void AlphaPreset_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Button btn && int.TryParse(btn.Tag?.ToString(), out int preset))
                SetAlphaPreset(preset);
        }

        private void SetAlphaPreset(int percent)
        {
            AlphaSlider.Value = percent;
        }

        private void RefreshOnionSkinPreview()
        {
            if (!_overlay.IsEnabled)
            {
                ClearOnionSkinLayers();
                return;
            }

            var recentFrames = _capture.GetRecentFrames(Math.Min(_overlay.OnionLayers, _onionSkinLayers.Length));
            for (int i = 0; i < _onionSkinLayers.Length; i++)
            {
                if (i < recentFrames.Count)
                {
                    _onionSkinLayers[i].Source = recentFrames[i].PreviewFrame;
                    _onionSkinLayers[i].Opacity = _overlay.GetAlphaForLayer(i + 1);
                    _onionSkinLayers[i].Visibility = Visibility.Visible;
                }
                else
                {
                    _onionSkinLayers[i].Source = null;
                    _onionSkinLayers[i].Visibility = Visibility.Collapsed;
                }
            }
        }

        private void ClearOnionSkinLayers()
        {
            foreach (var layer in _onionSkinLayers)
            {
                layer.Source = null;
                layer.Visibility = Visibility.Collapsed;
            }
        }

        private void PrevFrame_Click(object sender, RoutedEventArgs e)
        {
            if (_capture.Frames.Count == 0)
                return;

            StopPlaybackInternal();
            int targetIndex = _playbackIndex >= 0
                ? Math.Max(_playbackIndex - 1, 0)
                : GetNearestCaptureIndexAtOrBefore(_timelineCursorFrame - 1);
            ShowFrameAtPlaybackIndex(targetIndex);
            SetStatus($"Zum vorherigen Keyframe: Frame {_timelineCursorFrame}");
        }

        private void NextFrame_Click(object sender, RoutedEventArgs e)
        {
            if (_capture.Frames.Count == 0)
                return;

            StopPlaybackInternal();
            int targetIndex = _playbackIndex >= 0
                ? Math.Min(_playbackIndex + 1, _capture.Frames.Count - 1)
                : GetNearestCaptureIndexAtOrAfter(_timelineCursorFrame + 1);
            ShowFrameAtPlaybackIndex(targetIndex);
            SetStatus($"Zum nächsten Keyframe: Frame {_timelineCursorFrame}");
        }

        private void PlayPause_Click(object sender, RoutedEventArgs e)
        {
            if (_capture.Frames.Count == 0)
                return;

            if (_playbackTimer.IsEnabled)
            {
                StopPlaybackInternal();
                RefreshTimelineState();
                SetStatus("Playback pausiert.");
                return;
            }

            if (_playbackIndex < 0)
                _playbackIndex = GetNearestCaptureIndexAtOrAfter(_timelineCursorFrame);

            ShowFrameAtPlaybackIndex(_playbackIndex);
            _playbackTimer.Start();
            PlayPauseButton.Content = "⏸ Pause";
            RefreshTimelineState();
            SetStatus($"Playback gestartet mit {_playbackFps} fps.");
        }

        private void PlaybackTimer_Tick(object? sender, EventArgs e)
        {
            if (_capture.Frames.Count == 0)
            {
                StopPlaybackInternal();
                HidePlaybackPreview();
                RefreshTimelineState();
                return;
            }

            int nextIndex = (_playbackIndex + 1) % _capture.Frames.Count;
            ShowFrameAtPlaybackIndex(nextIndex);
        }

        private void ShowFrameAtPlaybackIndex(int index)
        {
            if (index < 0 || index >= _capture.Frames.Count)
                return;

            _playbackIndex = index;
            CapturedFrame frame = _capture.Frames[index];
            _timelineCursorFrame = frame.Index;
            ShowPlaybackFrame(frame);
            RefreshTimelineState();
            EnsureTimelineCursorVisible();
        }

        private void ShowPlaybackFrame(CapturedFrame frame)
        {
            PlaybackImage.Source = frame.PreviewFrame;
            PlaybackImage.Visibility = Visibility.Visible;
            NoCameraText.Visibility = Visibility.Collapsed;
        }

        private void HidePlaybackPreview()
        {
            PlaybackImage.Source = null;
            PlaybackImage.Visibility = Visibility.Collapsed;
        }

        private void StopPlaybackInternal()
        {
            if (_playbackTimer.IsEnabled)
                _playbackTimer.Stop();

            PlayPauseButton.Content = "▶ Abspielen";
        }

        private void ConnectStreamDeck_Click(object sender, RoutedEventArgs e)
        {
            bool connected = _streamDeck.Connect();

            if (connected)
            {
                StreamDeckStatusText.Text = "● Verbunden";
                StreamDeckStatusText.Foreground = new System.Windows.Media.SolidColorBrush(
                    System.Windows.Media.Color.FromRgb(0x4C, 0xAF, 0x50));
                SetStatus("Stream Deck verbunden und bereit.");
            }
            else
            {
                StreamDeckStatusText.Text = "● Nicht gefunden";
                MessageBox.Show(
                    "Kein Stream Deck gefunden.\nBitte Gerät anschließen und erneut versuchen.",
                    "Stream Deck",
                    MessageBoxButton.OK,
                    MessageBoxImage.Information);
            }
        }

        private void OpenCameraSettingsButton_Click(object sender, RoutedEventArgs e)
        {
            IntPtr ownerHandle = new WindowInteropHelper(this).Handle;
            bool opened = _camera.OpenDeviceSettings(ownerHandle);

            if (opened)
            {
                SetStatus("Kameraeinstellungen geoeffnet.");
                return;
            }

            MessageBox.Show(
                "Diese Kamera bietet keinen steuerbaren Webcam-/Treiberdialog an.",
                "Kamera-Steuerung",
                MessageBoxButton.OK,
                MessageBoxImage.Information);
        }

        private void SelectFolder_Click(object sender, RoutedEventArgs e)
        {
            string? folder = BrowseForFolder("Projektordner auswählen");
            if (string.IsNullOrWhiteSpace(folder))
                return;

            _capture.SetOutputFolder(folder);
            ProjectFolderText.Text = folder;
            UpdateShotPreview();
            SetStatus($"Projektordner: {folder}");
        }

        private void CaptureFormatComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!_uiReady || CaptureFormatComboBox.SelectedIndex < 0)
                return;

            CaptureOutputMode mode = CaptureFormatComboBox.SelectedIndex switch
            {
                1 => CaptureOutputMode.TiffWithProxy,
                _ => CaptureOutputMode.JpegSequence
            };

            _capture.SetOutputMode(mode);
            UpdateShotPreview();
            SetStatus(mode == CaptureOutputMode.TiffWithProxy
                ? "Capture-Format: TIFF-Master + JPEG-Proxies"
                : "Capture-Format: JPEG-Sequenz");
        }

        private void ApplyShotName_Click(object sender, RoutedEventArgs e) => BeginShot();

        private void ShotNameTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key != Key.Enter)
                return;

            BeginShot();
            e.Handled = true;
        }

        private void BeginShot()
        {
            StopPlaybackInternal();
            HidePlaybackPreview();
            _playbackIndex = -1;
            _timelineCursorFrame = 1;

            string shotName = _capture.BeginShot(ShotNameTextBox.Text);
            ShotNameTextBox.Text = shotName;
            UpdateFrameCounterText();

            RefreshOnionSkinPreview();
            UpdateShotPreview();
            RefreshTimelineState();
            SetStatus($"Shot aktiv: {shotName}");
        }

        private void UpdateShotPreview()
        {
            ShotPreviewText.Text = _capture.GetNextCapturePreview();
            WorkflowInfoText.Text = _capture.GetWorkflowDescription();
        }

        private void UpdateCameraSettingsButtonState()
        {
            OpenCameraSettingsButton.IsEnabled = _camera.IsRunning && _camera.CanOpenDeviceSettings;
        }

        private void SetStatus(string message)
        {
            StatusBarText.Text = message;
        }

        private void UpdateFrameCounterText()
        {
            FrameCounterText.Text = _capture.FrameCount == 0
                ? "Frame: 0"
                : $"Frame: {_capture.LastFrameNumber}";
        }

        private string? BrowseForFolder(string title)
        {
            var dialog = new OpenFileDialog
            {
                Title = title,
                CheckFileExists = false,
                CheckPathExists = true,
                FileName = "Ordner auswählen",
                Filter = "Ordner|*.none",
                ValidateNames = false
            };

            if (dialog.ShowDialog() != true)
                return null;

            return System.IO.Path.GetDirectoryName(dialog.FileName);
        }

        protected override void OnClosed(EventArgs e)
        {
            StopPlaybackInternal();
            _camera.Stop();
            _camera.Dispose();
            _streamDeck.Disconnect();
            base.OnClosed(e);
        }
    }
}
