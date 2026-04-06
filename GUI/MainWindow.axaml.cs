using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Controls.Primitives;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Media.Imaging;
using Avalonia.Threading;
using OpenStopMotionStudio.Core;
using System;
using System.Linq;
using Avalonia.Platform.Storage;
using System.Threading.Tasks;
using Avalonia.Platform;
using Avalonia.Media;
using System.Reflection;
using System.Resources;

namespace OpenStopMotionStudio.GUI
{
    public partial class MainWindow : Window
    {
        private readonly CameraManager _camera = CameraManager.Instance;
        private readonly CaptureManager _capture = new();
        private readonly OverlayManager _overlay = new();
        private readonly StreamDeckManager _streamDeck = new();
        private readonly DispatcherTimer _playbackTimer = new();
        private readonly ResourceManager _resourceManager = new("OpenStopMotionStudio.Localization.Strings", typeof(MainWindow).Assembly);
        private readonly ProjectMigrationService _projectMigration = new();

        private Image[] _onionSkinLayers = Array.Empty<Image>();
        private List<CameraDeviceDescriptor> _cameraDevices = new();
        private bool _uiReady;
        private bool _isRefreshingCameraList;
        private int _playbackIndex = -1;
        
        // Track last key press time to prevent auto-repeat spam (debounce 100ms)
        private readonly Dictionary<Key, long> _lastKeyPressTimes = new();
        private const long DEBOUNCE_MS = 50;

        public MainWindow(string projectPath)
        {
            DebugLogger.Instance.SetLogDirectory(ProjectPaths.GetLogsFolder(projectPath));

            InitializeComponent();
            _capture.SetOutputFolder(projectPath);
            BootstrapApplication();
            RestoreProjectState(projectPath);
        }

        public MainWindow()
        {
            DebugLogger.Instance.SetLogDirectory(Path.Combine(AppContext.BaseDirectory, "Logs"));

            InitializeComponent();
            BootstrapApplication();
        }

        private void BootstrapApplication()
        {
            var version = Assembly.GetExecutingAssembly().GetName().Version;
            string appVersion = $"v{version?.Major}.{version?.Minor}.{version?.Build}";
            Title = $"Open Stop Motion Studio {appVersion}";
            TitleText.Text = $"▶ Open Stop Motion Studio {appVersion}";
            
            // Log startup information
            DebugLogger.Instance.LogInfo("Startup", $"Open Stop Motion Studio {appVersion} initialized");
            DebugLogger.Instance.LogInfo("Startup", $"Log file: {DebugLogger.Instance.GetLogFilePath()}");
            
            _onionSkinLayers = new[] { OnionSkinImageLayer1, OnionSkinImageLayer2, OnionSkinImageLayer3 };

            _camera.FrameReady += OnFrameReady;
            _camera.ImageCaptured += OnCameraImageCaptured;
            _camera.StatusChanged += OnCameraStatusChanged;

            _streamDeck.CapturePressed += () => Dispatcher.UIThread.Post(TriggerCapture);
            _streamDeck.OnionTogglePressed += () => Dispatcher.UIThread.Post(ToggleOnionSkin);
            _streamDeck.AlphaPresetPressed += value => Dispatcher.UIThread.Post(() => SetAlphaPreset(value));
            _streamDeck.UndoPressed += () => Dispatcher.UIThread.Post(UndoLastCapture);

            _playbackTimer.Tick += PlaybackTimer_Tick;

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
            RefreshTimelineState();

            _uiReady = true;
            _ = RefreshCameraListAsync();
            
            // Register keyboard handlers on TimelineScrollViewer
            TimelineScrollViewer.KeyDown += TimelineScrollViewer_KeyDown;
            TimelineScrollViewer.KeyUp += MainWindow_KeyUp;
            
            // Register mouse wheel handler to intercept scroll events and convert to frame navigation
            TimelineScrollViewer.AddHandler(
                InputElement.PointerWheelChangedEvent,
                TimelineScrollViewer_PointerWheelChanged,
                handledEventsToo: true);
            
            // Register keyboard handler with handledEventsToo to capture all key events even when other controls have focus
            AddHandler(
                InputElement.KeyDownEvent,
                new EventHandler<KeyEventArgs>(MainWindow_GlobalKeyDown),
                handledEventsToo: true);
            
            // Register KeyUp handler to track key releases
            AddHandler(
                InputElement.KeyUpEvent,
                new EventHandler<KeyEventArgs>(MainWindow_KeyUp),
                handledEventsToo: true);
            
            // Set initial focus to timeline
            TimelineScrollViewer.Focus();
            
            DebugLogger.Instance.LogInfo("Startup", "Keyboard and mouse handlers registered. Auto-repeat debouncing enabled.");
            SetStatus("Bereit. Kamera auswählen und starten.");
        }

        private async Task RefreshCameraListAsync()
        {
            if (_isRefreshingCameraList)
                return;

            _isRefreshingCameraList = true;
            CameraComboBox.Items.Clear();
            CameraComboBox.Items.Add("Kameras werden gesucht...");
            CameraComboBox.SelectedIndex = 0;
            CameraComboBox.IsEnabled = false;
            StartCameraButton.IsEnabled = false;

            try
            {
                List<CameraDeviceDescriptor> devices = _camera.GetAvailableDevices();

                _cameraDevices = devices;
                CameraComboBox.Items.Clear();

                if (_cameraDevices.Count == 0)
                {
                    CameraComboBox.Items.Add(_resourceManager.GetString("NoCameraFound") ?? "Keine Kamera gefunden");
                    CameraComboBox.SelectedIndex = 0;
                    CameraComboBox.IsEnabled = false;
                    StartCameraButton.IsEnabled = false;
                    return;
                }

                foreach (var device in _cameraDevices)
                    CameraComboBox.Items.Add(device.DisplayName);

                CameraComboBox.IsEnabled = true;
                CameraComboBox.SelectedIndex = 0;
                StartCameraButton.IsEnabled = true;
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("CameraRefresh", $"Error refreshing camera list: {ex.Message}");
                CameraComboBox.Items.Clear();
                CameraComboBox.Items.Add("Fehler beim Laden der Kameras");
                CameraComboBox.SelectedIndex = 0;
                CameraComboBox.IsEnabled = false;
                StartCameraButton.IsEnabled = false;
            }
            finally
            {
                _isRefreshingCameraList = false;
            }
        }

        private void CameraComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!_uiReady)
                return;

            if (_camera.IsRunning)
            {
                _camera.Stop();
                StartCameraButton.Content = _resourceManager.GetString("StartCameraButton_Content_Start") ?? "▶ Kamera starten";
                CaptureButton.IsEnabled = false;
                LiveFeedImage.Source = null;
                NoCameraText.Text = _resourceManager.GetString("CameraChanged_Message") ?? "Kamera geändert.\nZum Fortfahren erneut starten.";
                NoCameraText.IsVisible = true;
                ResetHistogramPreview(_resourceManager.GetString("CameraSwitched_Message") ?? "Kamera gewechselt");
                SetStatus(_resourceManager.GetString("CameraSelectionChanged_Message") ?? "Kameraauswahl geändert.");
            }

            StopPlaybackInternal();
            HidePlaybackPreview();
            _playbackIndex = -1;
            RefreshTimelineState();
            
            // Load available resolutions for selected camera
            if (_uiReady)
            {
                _ = LoadAvailableResolutions();
            }
        }

        private async Task LoadAvailableResolutions()
        {
            try
            {
                ResolutionComboBox.Items.Clear();
                int cameraIndex = CameraComboBox.SelectedIndex;
                
                if (cameraIndex < 0)
                {
                    ResolutionComboBox.Items.Add("Keine Kamera ausgewählt");
                    ResolutionComboBox.SelectedIndex = 0;
                    ResolutionComboBox.IsEnabled = false;
                    return;
                }

                DebugLogger.Instance.LogInfo("Resolution", $"Loading resolutions for camera index {cameraIndex}");
                
                // Get standard resolutions (no hardware probing)
                var resolutions = _camera.GetSupportedResolutions(cameraIndex);
                
                if (resolutions == null || resolutions.Count == 0)
                {
                    DebugLogger.Instance.LogInfo("Resolution", "No resolutions available");
                    ResolutionComboBox.Items.Add("Keine Auflösungen verfügbar");
                    ResolutionComboBox.SelectedIndex = 0;
                    ResolutionComboBox.IsEnabled = false;
                    return;
                }

                // Update UI with resolutions
                ResolutionComboBox.IsEnabled = true;
                foreach (var resolution in resolutions)
                {
                    ResolutionComboBox.Items.Add(resolution);
                    DebugLogger.Instance.LogInfo("Resolution", $"Added: {resolution}");
                }

                var preferredResolution = _camera.RequestedResolution
                    ?? resolutions.FirstOrDefault(r => r.Width == 640 && r.Height == 480)
                    ?? resolutions.First();

                int preferredIndex = resolutions.FindIndex(r => r.Equals(preferredResolution));
                ResolutionComboBox.SelectedIndex = preferredIndex >= 0 ? preferredIndex : 0;
                SetStatus($"Auflösungen geladen: {resolutions.Count} verfügbar");
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogInfo("ResolutionLoad", $"Error loading resolutions: {ex.Message}");
                ResolutionComboBox.Items.Add("Fehler beim Laden");
                ResolutionComboBox.SelectedIndex = 0;
                ResolutionComboBox.IsEnabled = false;
            }
        }

        private void ResolutionComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!_uiReady || _camera.IsRunning)
                return;

            if (ResolutionComboBox.SelectedItem is CameraResolution resolution)
            {
                _camera.SetRequestedResolution(resolution.Width, resolution.Height);
                SetStatus($"Auflösung ausgewählt: {resolution}");
                DebugLogger.Instance.LogInfo("Resolution", $"Selected resolution: {resolution.Width}x{resolution.Height}");
            }
        }

        private async void StartCameraButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                StopPlaybackInternal();
                HidePlaybackPreview();
                _playbackIndex = -1;

                if (_camera.IsRunning)
                {
                    _camera.Stop();
                    StartCameraButton.Content = _resourceManager.GetString("StartCameraButton_Content_Start") ?? "▶ Kamera starten";
                    CaptureButton.IsEnabled = false;
                    LiveFeedImage.Source = null;
                    NoCameraText.Text = _resourceManager.GetString("NoCameraConnected_Message") ?? "Keine Kamera verbunden.\nKamera in der Sidebar auswählen.";
                    NoCameraText.IsVisible = true;
                    ResetHistogramPreview();
                    RefreshTimelineState();
                    SetStatus(_resourceManager.GetString("CameraStopped_Message") ?? "Kamera gestoppt.");
                    return;
                }

                int selectedIndex = CameraComboBox.SelectedIndex;
                if (selectedIndex < 0)
                    return;

                bool started = _camera.Start(selectedIndex);
                if (started)
                {
                    StartCameraButton.Content = _resourceManager.GetString("StartCameraButton_Content_Stop") ?? "⏹ Kamera stoppen";
                    CaptureButton.IsEnabled = true;
                    LiveFeedImage.Source = null;
                    NoCameraText.Text = _resourceManager.GetString("ConnectingCamera_Message") ?? "Verbinde Kamera...\nWarte auf erstes Bild.";
                    NoCameraText.IsVisible = true;
                    ResetHistogramPreview(_resourceManager.GetString("WaitingForLiveImage_Message") ?? "Warte auf Live-Bild");
                    RefreshTimelineState();
                    SetStatus(string.Format(_resourceManager.GetString("CameraActive_Message") ?? "Kamera aktiv: {0}", CameraComboBox.SelectedItem));
                }
                else
                {
                    string baseMessage = _resourceManager.GetString("CameraCouldNotBeStarted_Message") ?? "Kamera konnte nicht gestartet werden.";
                    string message = string.IsNullOrWhiteSpace(_camera.LastStatusMessage)
                        ? baseMessage
                        : $"{baseMessage}\n\n{_camera.LastStatusMessage}";

                    await MessageBox.Show(this, _resourceManager.GetString("Error_Title") ?? "Fehler", message);
                }
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("StartCameraButton_Click", $"Error starting camera: {ex.Message}");
                await MessageBox.Show(this, _resourceManager.GetString("Error_Title") ?? "Fehler", $"Kamera konnte nicht gestartet werden: {ex.Message}");
            }
        }

        private void OnFrameReady(Bitmap liveFrame)
        {
            Dispatcher.UIThread.Post(() =>
            {
                try
                {
                    if (!_camera.IsRunning)
                        return;

                    if (LiveFeedImage.Source != liveFrame)
                        LiveFeedImage.Source = liveFrame;

                    LiveFeedImage.InvalidateVisual();
                    NoCameraText.IsVisible = false;
                    RefreshOnionSkinPreview();
                    RefreshHistogramPreview(liveFrame);
                    RefreshOverlayCanvas();
                }
                catch (Exception ex)
                {
                    DebugLogger.Instance.LogError("OnFrameReady", $"Error rendering live frame: {ex.Message}");
                }
            });
        }

        private void CaptureButton_Click(object sender, RoutedEventArgs e) => TriggerCapture();

        private void TimelineScrollViewer_KeyDown(object? sender, KeyEventArgs e)
        {
            DebugLogger.Instance.LogKeyDown($"TimelineScrollViewer: {e.Key}");
            HandleArrowKeys(e);
        }

        private void MainWindow_GlobalKeyDown(object? sender, KeyEventArgs e)
        {
            DebugLogger.Instance.LogKeyDown($"Global: {e.Key}");
            DebugLogger.Instance.LogInfo("KeyDown", $"Key: {e.Key}, Handled: {e.Handled}");
            HandleArrowKeys(e);
        }

        private void HandleArrowKeys(KeyEventArgs e)
        {
            // Prevent auto-repeat spam using timestamp debouncing
            long now = DateTime.UtcNow.Ticks / TimeSpan.TicksPerMillisecond;
            
            if (_lastKeyPressTimes.TryGetValue(e.Key, out long lastTime))
            {
                if (now - lastTime < DEBOUNCE_MS)
                {
                    DebugLogger.Instance.LogInfo("KeyDown", $"Key: {e.Key} debounced (only {now - lastTime}ms ago)");
                    return;
                }
            }
            
            // Update last press time
            _lastKeyPressTimes[e.Key] = now;

            if (e.Key == Key.Space && CaptureButton.IsEnabled)
            {
                DebugLogger.Instance.Log("[ACTION] Capture triggered by Space key");
                TriggerCapture();
                e.Handled = true;
            }
            else if (e.Key == Key.Left && !e.Handled)
            {
                // Navigate to previous frame with left arrow key
                DebugLogger.Instance.Log("[ACTION] Left arrow pressed, navigating to previous frame");
                StopPlaybackInternal();
                int nextFrame = Math.Max(_timelineCursorFrame - 1, 1);
                DebugLogger.Instance.LogFrameNavigation(_timelineCursorFrame, nextFrame, "Left Arrow");
                MoveTimelineCursorToFrame(nextFrame, true);
                e.Handled = true;
            }
            else if (e.Key == Key.Right && !e.Handled)
            {
                // Navigate to next frame with right arrow key
                DebugLogger.Instance.Log("[ACTION] Right arrow pressed, navigating to next frame");
                StopPlaybackInternal();
                int nextFrame = Math.Min(_timelineCursorFrame + 1, GetTimelineEndFrame());
                DebugLogger.Instance.LogFrameNavigation(_timelineCursorFrame, nextFrame, "Right Arrow");
                MoveTimelineCursorToFrame(nextFrame, true);
                e.Handled = true;
            }
        }

        private void MainWindow_KeyUp(object? sender, KeyEventArgs e)
        {
            // Clear debounce time on key release to allow new key presses
            _lastKeyPressTimes.Remove(e.Key);
            DebugLogger.Instance.LogInfo("KeyUp", $"Key: {e.Key} released");
        }

        private void TimelineScrollViewer_PointerPressed(object? sender, PointerPressedEventArgs e)
        {
            // Set focus to timeline when clicked so keyboard events are received
            TimelineScrollViewer.Focus();
            DebugLogger.Instance.Log("[ACTION] Focus set to TimelineScrollViewer");
        }

        private void TimelineCanvas_PointerPressed(object? sender, PointerPressedEventArgs e)
        {
            // Set focus to timeline when canvas is clicked
            TimelineScrollViewer.Focus();
            DebugLogger.Instance.Log("[ACTION] Focus set to TimelineScrollViewer via Canvas");
        }

        private void TriggerCapture()
        {
            StopPlaybackInternal();
            HidePlaybackPreview();
            _playbackIndex = -1;

            if (IsSdkStillCaptureActive())
            {
                if (_camera.TriggerHardwareCapture())
                    SetStatus("Hardware-Ausloeser ausgelöst. Warte auf das Kamerabild...");
                else
                    SetStatus("Hardware-Ausloesen fehlgeschlagen.");
                return;
            }

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

        private void OnCameraImageCaptured(string filePath)
        {
            Dispatcher.UIThread.Post(() =>
            {
                try
                {
                    Bitmap? previewFallback = _camera.GetCurrentFrame();
                    CapturedFrame capturedFrame = _capture.ImportCapturedFile(filePath, previewFallback, moveSourceFile: true);
                    _timelineCursorFrame = capturedFrame.Index;

                    UpdateFrameCounterText();
                    RefreshOnionSkinPreview();
                    UpdateShotPreview();
                    RefreshTimelineState();
                    EnsureTimelineCursorVisible();

                    PlaybackStatusText.Text = $"Capture-Keyframe: Frame {capturedFrame.Index} importiert";
                    SetStatus($"✔ DSLR-Frame {capturedFrame.Index} importiert: {System.IO.Path.GetFileName(capturedFrame.MasterPath)}");
                }
                catch (Exception ex)
                {
                    DebugLogger.Instance.LogError("OnCameraImageCaptured", $"Error importing hardware capture: {ex.Message}");
                    SetStatus($"Hardware-Capture konnte nicht importiert werden: {ex.Message}");
                }
            });
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

        private void OnionSkinToggle_IsCheckedChanged(object sender, RoutedEventArgs e)
        {
            if (OnionSkinToggle.IsChecked == true)
            {
                _overlay.IsEnabled = true;
                OnionSkinToggle.Content = "Onion Skin: AN ✓";
                RefreshOnionSkinPreview();
            }
            else
            {
                _overlay.IsEnabled = false;
                OnionSkinToggle.Content = "Onion Skin: AUS";
                ClearOnionSkinLayers();
            }
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

        private void AlphaSlider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
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

        private void GridToggle_IsCheckedChanged(object sender, RoutedEventArgs e)
        {
            if (!_uiReady)
                return;

            _overlay.ShowGrid = GridToggle.IsChecked == true;
            RefreshOverlayCanvas();
            SetStatus($"Grid Overlay: {(_overlay.ShowGrid ? "AN ✓" : "AUS")}");
        }

        private void ActionSafeToggle_IsCheckedChanged(object sender, RoutedEventArgs e)
        {
            if (!_uiReady)
                return;

            _overlay.ShowActionSafe = ActionSafeToggle.IsChecked == true;
            RefreshOverlayCanvas();
            SetStatus($"Action Safe Zone: {(_overlay.ShowActionSafe ? "AN ✓" : "AUS")}");
        }

        private void TitleSafeToggle_IsCheckedChanged(object sender, RoutedEventArgs e)
        {
            if (!_uiReady)
                return;

            _overlay.ShowTitleSafe = TitleSafeToggle.IsChecked == true;
            RefreshOverlayCanvas();
            SetStatus($"Title Safe Zone: {(_overlay.ShowTitleSafe ? "AN ✓" : "AUS")}");
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
                    _onionSkinLayers[i].IsVisible = true;
                }
                else
                {
                    _onionSkinLayers[i].Source = null;
                    _onionSkinLayers[i].IsVisible = false;
                }
            }
        }

        private void ClearOnionSkinLayers()
        {
            foreach (var layer in _onionSkinLayers)
            {
                layer.Source = null;
                layer.IsVisible = false;
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
            PlaybackImage.IsVisible = true;
            NoCameraText.IsVisible = false;
        }

        private void HidePlaybackPreview()
        {
            PlaybackImage.Source = null;
            PlaybackImage.IsVisible = false;
        }

        private void StopPlaybackInternal()
        {
            if (_playbackTimer.IsEnabled)
                _playbackTimer.Stop();

            PlayPauseButton.Content = "▶ Abspielen";
        }

        private async void ConnectStreamDeck_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                bool connected = _streamDeck.Connect();

                if (connected)
                {
                    StreamDeckStatusText.Text = "● Verbunden";
                    StreamDeckStatusText.Foreground = SolidColorBrush.Parse("#4CAF50");
                    SetStatus("Stream Deck verbunden und bereit.");
                }
                else
                {
                    StreamDeckStatusText.Text = "● Nicht gefunden";
                    await MessageBox.Show(this, "Stream Deck", "Kein Stream Deck gefunden.\nBitte Gerät anschließen und erneut versuchen.");
                }
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("ConnectStreamDeck_Click", $"Error connecting to Stream Deck: {ex.Message}");
                StreamDeckStatusText.Text = "● Fehler";
                await MessageBox.Show(this, "Stream Deck", $"Fehler bei der Verbindung zum Stream Deck: {ex.Message}");
            }
        }

        private async void SelectFolder_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string? folder = await BrowseForFolder("Projektordner auswählen");
                if (string.IsNullOrWhiteSpace(folder))
                    return;

                _capture.SetOutputFolder(folder);
                ProjectFolderText.Text = folder;
                UpdateShotPreview();
                SetStatus($"Projektordner: {folder}");
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("SelectFolder_Click", $"Error selecting folder: {ex.Message}");
                await MessageBox.Show(this, "Fehler", $"Fehler bei der Ordnerauswahl: {ex.Message}");
            }
        }

        private void CaptureFormatComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!_uiReady || CaptureFormatComboBox.SelectedIndex < 0)
                return;

            CaptureOutputMode mode = CaptureFormatComboBox.SelectedIndex switch
            {
                1 => CaptureOutputMode.PngWithProxy,
                _ => CaptureOutputMode.JpegSequence
            };

            _capture.SetOutputMode(mode);
            UpdateShotPreview();
            RefreshOverlayCanvas();
            SetStatus(mode == CaptureOutputMode.PngWithProxy
                ? "Capture-Format: PNG-Master + JPEG-Proxies"
                : "Capture-Format: JPEG-Master");
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

            string shotName = _capture.BeginShot(ShotNameTextBox.Text ?? "frame");
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

        private void SetStatus(string message)
        {
            StatusBarText.Text = message;
        }

        private void OnCameraStatusChanged(string message)
        {
            Dispatcher.UIThread.Post(() =>
            {
                SetStatus(message);
                DebugLogger.Instance.LogInfo("Camera", message);
            });
        }

        private void UpdateFrameCounterText()
        {
            FrameCounterText.Text = _capture.FrameCount == 0
                ? "Frame: 0"
                : $"Frame: {_capture.LastFrameNumber}";
        }

        private async Task<string?> BrowseForFolder(string title)
        {
            var storageProvider = this.StorageProvider;
            var result = await storageProvider.OpenFolderPickerAsync(new FolderPickerOpenOptions
            {
                Title = title,
                AllowMultiple = false
            });

            if (result.Count >= 1)
            {
                return result[0].Path.LocalPath;
            }

            return null;
        }

        protected override void OnClosed(EventArgs e)
        {
            StopPlaybackInternal();
            _camera.ImageCaptured -= OnCameraImageCaptured;
            _camera.StatusChanged -= OnCameraStatusChanged;
            _camera.Stop();
            _camera.Dispose();
            _streamDeck.Disconnect();
            base.OnClosed(e);
        }

        private bool IsSdkStillCaptureActive()
        {
            return _camera.IsRunning && _camera.UsesHardwareStillCapture;
        }

        private async void RestoreProjectState(string projectPath)
        {
            try
            {
                SetStatus($"Lade Projekt: {projectPath}...");
                
                var (migration, loadSummary) = await Task.Run(() =>
                {
                    var migrationReport = _projectMigration.Migrate(projectPath);
                    var summary = _capture.LoadProjectFramesFromDisk();
                    return (migrationReport, summary);
                });

                await Dispatcher.UIThread.InvokeAsync(() =>
                {
                    ProjectFolderText.Text = projectPath;
                    if (loadSummary is not null && loadSummary.LoadedFrameCount > 0)
                    {
                        _capture.LoadImportedFrames(loadSummary.ShotName, loadSummary.FrameStart, loadSummary.Frames);
                        ShotNameTextBox.Text = loadSummary.ShotName;
                        _timelineCursorFrame = loadSummary.Frames[^1].Index;

                        UpdateFrameCounterText();
                        UpdateShotPreview();
                        RefreshOnionSkinPreview();
                        RefreshTimelineState();
                        EnsureTimelineCursorVisible();
                        ShowFrameAtPlaybackIndex(loadSummary.LoadedFrameCount - 1);

                        string migrationText = migration.HasChanges
                            ? $" | Migration: {migration.MovedFiles} Dateien"
                            : string.Empty;
                        SetStatus($"Projekt geladen: {projectPath} | Shot {loadSummary.ShotName} mit {loadSummary.LoadedFrameCount} Frames{migrationText}");
                    }
                    else
                    {
                        UpdateShotPreview();
                        RefreshTimelineState();
                        if (migration.HasChanges)
                        {
                            SetStatus($"Projekt geladen: {projectPath} | Migration abgeschlossen: {migration.MovedFiles} Dateien verschoben");
                        }
                        else
                        {
                            SetStatus($"Projekt geladen: {projectPath}");
                        }
                    }
                });
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("ProjectLoad", $"Failed to load project '{projectPath}'. Reason: {ex.Message}");
                await Dispatcher.UIThread.InvokeAsync(async () =>
                {
                    await MessageBox.Show(this, "Fehler beim Laden des Projekts", $"Das Projekt '{Path.GetFileName(projectPath)}' konnte nicht geladen werden.\n\nUrsache: {ex.Message}");
                    
                    if (Application.Current?.ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
                    {
                        var projectWindow = new ProjectWindow(desktop);
                        projectWindow.Show();
                    }
                    Close();
                });
            }
        }
    }
}
