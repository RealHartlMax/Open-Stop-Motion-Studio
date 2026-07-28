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
using System.IO;
using System.Collections.Generic;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Threading;

namespace OpenStopMotionStudio.GUI
{
    public partial class MainWindow : Window
    {
        private readonly CameraManager _camera = CameraManager.Instance;
        private readonly CaptureManager _capture = new();
        private readonly OverlayManager _overlay = new();
        private readonly ReferenceOverlayProvider _referenceOverlay = new();
        private readonly StreamDeckManager _streamDeck = new();
        private readonly DispatcherTimer _playbackTimer = new();
        private readonly ResourceManager _resourceManager = new("OpenStopMotionStudio.Localization.Strings", typeof(MainWindow).Assembly);
        private CultureInfo _currentCulture = CultureInfo.GetCultureInfo("de-DE");
        private readonly ProjectMigrationService _projectMigration = new();
        private Bitmap? _referenceLoopIcon;
        private Bitmap? _referenceHoldIcon;
        private Bitmap? _tabCaptureIcon;
        private Bitmap? _tabOnionOverlayIcon;
        private Bitmap? _tabProjectRawIcon;
        private Bitmap? _tabHardwareIcon;
        private readonly Dictionary<string, Bitmap> _tintedOnionCache = new();
        private static readonly Color[] OnionTintPalette =
        {
            Color.Parse("#FF4A4A"),
            Color.Parse("#4A8CFF"),
            Color.Parse("#45D18A"),
            Color.Parse("#F7A531"),
            Color.Parse("#D86CFF")
        };

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
            ApplySavedLanguage();
            _capture.SetOutputFolder(projectPath);
            BootstrapApplication();
            RestoreProjectState(projectPath);
        }

        public MainWindow()
        {
            DebugLogger.Instance.SetLogDirectory(Path.Combine(AppContext.BaseDirectory, "Logs"));

            InitializeComponent();
            ApplySavedLanguage();
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
            
            _onionSkinLayers = new[]
            {
                OnionSkinImageLayer1,
                OnionSkinImageLayer2,
                OnionSkinImageLayer3,
                OnionSkinImageLayer4,
                OnionSkinImageLayer5
            };

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
            _overlay.ReferenceAlphaValue = ReferenceAlphaSlider.Value / 100.0;
            _overlay.LoopPlaybackAlphaValue = LoopOverlayAlphaSlider.Value / 100.0;
            ReferenceOverlayInfoText.Text = _referenceOverlay.SourceLabel;
            UpdateTabIcons();
            UpdateReferencePlaybackModeButton();
            ApplyUiTranslations();

            PlaybackFpsTextBox.Text = DefaultPlaybackFps.ToString();
            ApplyPlaybackFpsFromInput();
            ShotNameTextBox.Text = _capture.ShotName;
            ProjectFolderText.Text = _capture.OutputFolder;
            ResetHistogramPreview();
            UpdateShotPreview();
            InitializeRawImportUi();
            UpdateFrameCounterText();
            RefreshTimelineState();
            UpdateCameraIntegrationStatus();

            _uiReady = true;
            UpdateLanguageSelector();
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
            SetStatus(_resourceManager.GetString("StatusReadyMessage") ?? "Ready. Select a camera and start.");
        }

        private void ApplySavedLanguage()
        {
            string cultureName = Environment.GetEnvironmentVariable("OSMS_LANGUAGE") ?? "de-DE";
            if (!string.IsNullOrWhiteSpace(cultureName))
            {
                try
                {
                    _currentCulture = CultureInfo.GetCultureInfo(cultureName);
                }
                catch
                {
                    _currentCulture = CultureInfo.GetCultureInfo("de-DE");
                }
            }

            Thread.CurrentThread.CurrentUICulture = _currentCulture;
            Thread.CurrentThread.CurrentCulture = _currentCulture;
            CultureInfo.CurrentUICulture = _currentCulture;
            CultureInfo.CurrentCulture = _currentCulture;
        }

        private void UpdateLanguageSelector()
        {
            if (LanguageComboBox is null)
                return;

            LanguageComboBox.Items.Clear();
            LanguageComboBox.Items.Add(_resourceManager.GetString("LanguageGerman") ?? "German");
            LanguageComboBox.Items.Add(_resourceManager.GetString("LanguageEnglish") ?? "English");
            LanguageComboBox.SelectedIndex = _currentCulture.TwoLetterISOLanguageName.Equals("en", StringComparison.OrdinalIgnoreCase) ? 1 : 0;
        }

        private void ApplyLanguageSelection()
        {
            if (LanguageComboBox is null)
                return;

            int selectedIndex = LanguageComboBox.SelectedIndex;
            string cultureName = selectedIndex == 1 ? "en-US" : "de-DE";
            _currentCulture = CultureInfo.GetCultureInfo(cultureName);
            Thread.CurrentThread.CurrentUICulture = _currentCulture;
            Thread.CurrentThread.CurrentCulture = _currentCulture;
            CultureInfo.CurrentUICulture = _currentCulture;
            CultureInfo.CurrentCulture = _currentCulture;
            Environment.SetEnvironmentVariable("OSMS_LANGUAGE", cultureName);
            RefreshTranslatedUi();
        }

        private void RefreshTranslatedUi()
        {
            if (LanguageLabel is null || LanguageComboBox is null)
                return;

            LanguageLabel.Text = _resourceManager.GetString("LanguageLabel") ?? "Language";
            CameraSectionTitle?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("CameraSectionTitle") ?? "Camera");
            LanguageComboBox.Items.Clear();
            LanguageComboBox.Items.Add(_resourceManager.GetString("LanguageGerman") ?? "German");
            LanguageComboBox.Items.Add(_resourceManager.GetString("LanguageEnglish") ?? "English");
            LanguageComboBox.SelectedIndex = _currentCulture.TwoLetterISOLanguageName.Equals("en", StringComparison.OrdinalIgnoreCase) ? 1 : 0;

            PrevFrameButton.Content = _resourceManager.GetString("PrevFrameButton_Content") ?? "◀ Previous";
            PlayPauseButton.Content = _playbackTimer.IsEnabled
                ? _resourceManager.GetString("PauseButton_Content") ?? "⏸ Pause"
                : _resourceManager.GetString("PlayPauseButton_Content_Play") ?? "▶ Play";
            NextFrameButton.Content = _resourceManager.GetString("NextFrameButton_Content") ?? "Next ▶";
            PlaybackFpsHintText.Text = _resourceManager.GetString("PlaybackFpsHint") ?? "fps (1-120)";
            NoCameraText.Text = _resourceManager.GetString("NoCameraConnected_Message") ?? "No camera connected.\nSelect a camera in the sidebar.";
            ApplyUiTranslations();
            UpdatePlaybackSpeedLabel();
            UpdateFrameCounterText();

            if (_uiReady)
            {
                RefreshTimelineState();
                UpdateCameraIntegrationStatus();
            }

            SetStatus(_resourceManager.GetString("StatusReadyMessage") ?? "Ready. Select a camera and start.");
        }

        private void ApplyUiTranslations()
        {
            TimelineDopeSheetLabel?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("TimelineDopeSheetLabel") ?? "Dope Sheet");
            TimelineSummaryLabel?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("TimelineSummaryLabel") ?? "Summary");
            TimelineCaptureLabel?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("TimelineCaptureLabel") ?? "Capture");
            TimelineMotionCtrlLabel?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("TimelineMotionCtrlLabel") ?? "Motion Ctrl");
            TimelineDmxLightLabel?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("TimelineDmxLightLabel") ?? "DMX Licht");
            CaptureTabHeaderText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("CaptureTabHeader") ?? "Capture");
            OnionOverlayTabHeaderText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("OnionOverlayTabHeader") ?? "Onion & Overlay");
            ProjectRawTabHeaderText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("ProjectRawTabHeader") ?? "Project & RAW");
            HardwareTabHeaderText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("HardwareTabHeader") ?? "Hardware");
            ResolutionLabelText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("ResolutionLabel") ?? "Resolution");
            HistogramTitleText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("HistogramTitle") ?? "Histogram");
            CaptureSectionTitleText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("CaptureSectionTitle") ?? "📸 Capture");
            CaptureShortcutText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("CaptureShortcutText") ?? "Shortcut: Spacebar");
            OnionSectionTitleText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("OnionSectionTitle") ?? "🧅 Onion Skin");
            OnionLayerLabelText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("OnionLayerLabel") ?? "Layer");
            OnionLayer1ComboBoxItem?.SetValue(ComboBoxItem.ContentProperty, _resourceManager.GetString("OnionLayer1Label") ?? "1 Layer");
            OnionLayer2ComboBoxItem?.SetValue(ComboBoxItem.ContentProperty, _resourceManager.GetString("OnionLayer2Label") ?? "2 Layer");
            OnionLayer3ComboBoxItem?.SetValue(ComboBoxItem.ContentProperty, _resourceManager.GetString("OnionLayer3Label") ?? "3 Layer");
            OnionLayer4ComboBoxItem?.SetValue(ComboBoxItem.ContentProperty, _resourceManager.GetString("OnionLayer4Label") ?? "4 Layer");
            OnionLayer5ComboBoxItem?.SetValue(ComboBoxItem.ContentProperty, _resourceManager.GetString("OnionLayer5Label") ?? "5 Layer");
            FastPresetsLabelText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("FastPresetsLabel") ?? "Quick presets:");
            LoadReferenceOverlayButton?.SetValue(Button.ContentProperty, _resourceManager.GetString("LoadReferenceOverlayButton_Content") ?? "Load video/image");
            LoadReferenceSequenceButton?.SetValue(Button.ContentProperty, _resourceManager.GetString("LoadReferenceSequenceButton_Content") ?? "Load image sequence");
            RemoveReferenceButton?.SetValue(Button.ContentProperty, _resourceManager.GetString("RemoveReferenceButton_Content") ?? "Remove reference");
            CompositionOverlaysTitleText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("CompositionOverlaysTitle") ?? "📐 Composition overlays");
            ProjectSectionTitleText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("ProjectSectionTitle") ?? "📁 Project");
            CaptureFormatLabelText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("CaptureFormatLabel") ?? "Capture format");
            ShotNameLabelText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("ShotNameLabel") ?? "Shot name");
            ApplyShotButton?.SetValue(Button.ContentProperty, _resourceManager.GetString("ApplyShotButton_Content") ?? "Apply shot");
            SelectProjectFolderButton?.SetValue(Button.ContentProperty, _resourceManager.GetString("SelectProjectFolderButton_Content") ?? "Choose folder");
            NefImportSectionTitleText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("NefImportSectionTitle") ?? "NEF import");
            RawSourceLabelText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("RawSourceLabel") ?? "RAW source");
            StartFrameLabelText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("StartFrameLabel") ?? "Start frame");
            ProxyFormatLabelText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("ProxyFormatLabel") ?? "Proxy format");
            ImportInstructionsText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("ImportInstructionsText") ?? "Import: PNG master in Masters/<Shot>, proxy in Proxy/<Shot>");
            SelectRawSourceFolderButton?.SetValue(Button.ContentProperty, _resourceManager.GetString("SelectRawSourceFolderButton_Content") ?? "Choose NEF folder");
            CameraIntegrationTitleText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("CameraIntegrationTitle") ?? "📷 Camera integration");
            StreamDeckSectionTitleText?.SetValue(TextBlock.TextProperty, _resourceManager.GetString("StreamDeckSectionTitle") ?? "🎛 Stream Deck");
            ConnectStreamDeckButton?.SetValue(Button.ContentProperty, _resourceManager.GetString("ConnectStreamDeckButton_Content") ?? "Connect");
        }

        private async Task RefreshCameraListAsync()
        {
            if (_isRefreshingCameraList)
                return;

            _isRefreshingCameraList = true;
            CameraComboBox.Items.Clear();
            CameraComboBox.Items.Add(_resourceManager.GetString("CameraSearchMessage") ?? "Searching for cameras...");
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
                    CameraComboBox.Items.Add(_resourceManager.GetString("NoCameraFound") ?? "No camera found");
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
                UpdateCameraIntegrationStatus();
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("CameraRefresh", $"Error refreshing camera list: {ex.Message}");
                CameraComboBox.Items.Clear();
                CameraComboBox.Items.Add(_resourceManager.GetString("CameraLoadErrorMessage") ?? "Error loading cameras");
                CameraComboBox.SelectedIndex = 0;
                CameraComboBox.IsEnabled = false;
                StartCameraButton.IsEnabled = false;
                UpdateCameraIntegrationStatus();
            }
            finally
            {
                _isRefreshingCameraList = false;
            }
        }

        private void LanguageComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!_uiReady)
                return;

            ApplyLanguageSelection();
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

            UpdateCameraIntegrationStatus();

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
                    ResolutionComboBox.Items.Add(_resourceManager.GetString("NoCameraSelected") ?? "No camera selected");
                    ResolutionComboBox.SelectedIndex = 0;
                    ResolutionComboBox.IsEnabled = false;
                    ResolutionHintText.Text = _resourceManager.GetString("NoCameraSelectedHint") ?? "No camera selected.";
                    return;
                }

                CameraDeviceDescriptor? descriptor = GetSelectedCameraDescriptor();
                if (IsSdkBackedCamera(descriptor))
                {
                    ResolutionComboBox.Items.Add(_resourceManager.GetString("SdkResolutionMessage") ?? "SDK live view: native camera resolution");
                    ResolutionComboBox.SelectedIndex = 0;
                    ResolutionComboBox.IsEnabled = false;
                    ResolutionHintText.Text = _resourceManager.GetString("SdkResolutionHint") ?? "For SDK cameras, the native camera resolution is used. Manual selection is disabled here.";
                    return;
                }

                DebugLogger.Instance.LogInfo("Resolution", $"Loading resolutions for camera index {cameraIndex}");
                
                // Get standard resolutions (no hardware probing)
                var resolutions = _camera.GetSupportedResolutions(cameraIndex);
                
                if (resolutions == null || resolutions.Count == 0)
                {
                    DebugLogger.Instance.LogInfo("Resolution", "No resolutions available");
                    ResolutionComboBox.Items.Add(_resourceManager.GetString("NoResolutionsAvailable") ?? "No resolutions available");
                    ResolutionComboBox.SelectedIndex = 0;
                    ResolutionComboBox.IsEnabled = false;
                    ResolutionHintText.Text = _resourceManager.GetString("NoResolutionsAvailableHint") ?? "No manual resolutions were found for this camera.";
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
                ResolutionHintText.Text = _resourceManager.GetString("ResolutionBeforeStart") ?? "Choose a resolution before starting.";
                SetStatus(string.Format(_resourceManager.GetString("ResolutionLoadedMessage") ?? "Loaded {0} resolutions.", resolutions.Count));
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogInfo("ResolutionLoad", $"Error loading resolutions: {ex.Message}");
                ResolutionComboBox.Items.Add(_resourceManager.GetString("ResolutionLoadError") ?? "Error loading resolutions");
                ResolutionComboBox.SelectedIndex = 0;
                ResolutionComboBox.IsEnabled = false;
                ResolutionHintText.Text = _resourceManager.GetString("ResolutionLoadErrorHint") ?? "The resolutions could not be loaded.";
            }
        }

        private void ResolutionComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!_uiReady || _camera.IsRunning)
                return;

            if (ResolutionComboBox.SelectedItem is CameraResolution resolution)
            {
                _camera.SetRequestedResolution(resolution.Width, resolution.Height);
                SetStatus(string.Format(_resourceManager.GetString("ResolutionSelectedMessage") ?? "Resolution selected: {0}", resolution));
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
                    UpdateCameraIntegrationStatus();
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
                    UpdateCameraIntegrationStatus();
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
                await MessageBox.Show(this, _resourceManager.GetString("Error_Title") ?? "Fehler", string.Format(_resourceManager.GetString("CameraCouldNotBeStartedWithReason_Message") ?? "The camera could not be started: {0}", ex.Message));
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
                    RefreshReferenceOverlayPreview();
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
            TimelineScrollViewer.Focus();

            var point = e.GetCurrentPoint(TimelineCanvas);
            if (!point.Properties.IsLeftButtonPressed)
                return;

            _isTimelinePointerScrubbing = true;
            e.Pointer.Capture(TimelineCanvas);
            UpdateTimelineFromPointerPosition(point.Position, announce: false);
            DebugLogger.Instance.Log("[ACTION] Timeline pointer scrubbing started");
            e.Handled = true;
        }

        private void TimelineCanvas_PointerMoved(object? sender, PointerEventArgs e)
        {
            if (!_isTimelinePointerScrubbing)
                return;

            UpdateTimelineFromPointerPosition(e.GetPosition(TimelineCanvas), announce: false);
            e.Handled = true;
        }

        private void TimelineCanvas_PointerReleased(object? sender, PointerReleasedEventArgs e)
        {
            if (!_isTimelinePointerScrubbing)
                return;

            UpdateTimelineFromPointerPosition(e.GetPosition(TimelineCanvas), announce: true);
            _isTimelinePointerScrubbing = false;
            e.Pointer.Capture(null);
            DebugLogger.Instance.Log("[ACTION] Timeline pointer scrubbing finished");
            e.Handled = true;
        }

        private void TimelineCanvas_PointerCaptureLost(object? sender, PointerCaptureLostEventArgs e)
        {
            _isTimelinePointerScrubbing = false;
        }

        private void TriggerCapture()
        {
            StopPlaybackInternal();
            HidePlaybackPreview();
            _playbackIndex = -1;

            if (IsSdkStillCaptureActive())
            {
                if (_camera.TriggerHardwareCapture())
                    SetStatus(_resourceManager.GetString("HardwareTrigger_Success") ?? "Hardware trigger fired. Waiting for the camera image...");
                else
                    SetStatus(_resourceManager.GetString("HardwareTrigger_Failed") ?? "Hardware trigger failed.");
                return;
            }

            var currentFrame = _camera.GetCurrentFrame();
            if (currentFrame == null)
            {
                SetStatus(_resourceManager.GetString("NoFrameAvailableMessage") ?? "⚠ No frame available.");
                return;
            }

            CapturedFrame capturedFrame = _capture.SaveFrame(currentFrame);
            _timelineCursorFrame = capturedFrame.Index;
            UpdateFrameCounterText();

            RefreshOnionSkinPreview();
            RefreshReferenceOverlayPreview();
            UpdateShotPreview();
            RefreshTimelineState();
            EnsureTimelineCursorVisible();

            PlaybackStatusText.Text = string.Format(_resourceManager.GetString("CaptureKeyframeSavedMessage") ?? "Capture keyframe: Frame {0} saved", capturedFrame.Index);
            SetStatus(string.Format(_resourceManager.GetString("FrameSavedMessage") ?? "✔ Frame {0} saved: {1}", capturedFrame.Index, System.IO.Path.GetFileName(capturedFrame.MasterPath)));
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
                    RefreshReferenceOverlayPreview();
                    UpdateShotPreview();
                    RefreshTimelineState();
                    EnsureTimelineCursorVisible();

                    PlaybackStatusText.Text = string.Format(_resourceManager.GetString("CaptureKeyframeImportedMessage") ?? "Capture keyframe: Frame {0} imported", capturedFrame.Index);
                    SetStatus(string.Format(_resourceManager.GetString("DslrFrameImportedMessage") ?? "✔ DSLR frame {0} imported: {1}", capturedFrame.Index, System.IO.Path.GetFileName(capturedFrame.MasterPath)));
                }
                catch (Exception ex)
                {
                    DebugLogger.Instance.LogError("OnCameraImageCaptured", $"Error importing hardware capture: {ex.Message}");
                    SetStatus(string.Format(_resourceManager.GetString("HardwareCaptureImportFailedMessage") ?? "Hardware capture could not be imported: {0}", ex.Message));
                }
            });
        }

        private void UndoLastCapture()
        {
            if (!_capture.UndoLastCapture())
            {
                SetStatus(_resourceManager.GetString("UndoNoFramesMessage") ?? "No frame to undo.");
                return;
            }

            StopPlaybackInternal();
            HidePlaybackPreview();
            _playbackIndex = -1;

            _timelineCursorFrame = _capture.Frames.Count > 0 ? _capture.Frames[^1].Index : 1;

            UpdateFrameCounterText();
            RefreshOnionSkinPreview();
            RefreshReferenceOverlayPreview();
            UpdateShotPreview();
            RefreshTimelineState();
            EnsureTimelineCursorVisible();

            SetStatus(_capture.FrameCount > 0
                ? string.Format(_resourceManager.GetString("UndoRemovedFrameMessage") ?? "Last frame removed. Current: {0}", _capture.LastFrameNumber)
                : _resourceManager.GetString("UndoRemovedLastFrameMessage") ?? "Last frame removed. No frames left.");
        }

        private void OnionSkinToggle_IsCheckedChanged(object sender, RoutedEventArgs e)
        {
            if (OnionSkinToggle.IsChecked == true)
            {
                _overlay.IsEnabled = true;
                OnionSkinToggle.Content = _resourceManager.GetString("OnionSkinToggle_On") ?? "Onion Skin: ON ✓";
                RefreshOnionSkinPreview();
            }
            else
            {
                _overlay.IsEnabled = false;
                OnionSkinToggle.Content = _resourceManager.GetString("OnionSkinToggle_Off") ?? "Onion Skin: OFF";
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

        private void OnionColorCodedToggle_IsCheckedChanged(object? sender, RoutedEventArgs e)
        {
            _overlay.ColorCodedMode = OnionColorCodedToggle.IsChecked == true;
            RefreshOnionSkinPreview();
            SetStatus(_overlay.ColorCodedMode
                ? _resourceManager.GetString("OnionSkinColorCode_On") ?? "Onion Skin color coding: ON"
                : _resourceManager.GetString("OnionSkinColorCode_Off") ?? "Onion Skin color coding: OFF");
        }

        private void LoopPlaybackToggle_IsCheckedChanged(object? sender, RoutedEventArgs e)
        {
            _overlay.ShowLoopPlaybackOverlay = LoopPlaybackToggle.IsChecked == true;
            RefreshLoopPlaybackOverlay();
            SetStatus(_overlay.ShowLoopPlaybackOverlay
                ? _resourceManager.GetString("LoopPlayback_On") ?? "Loop comparison: ON"
                : _resourceManager.GetString("LoopPlayback_Off") ?? "Loop comparison: OFF");
        }

        private void AlphaSlider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (!_uiReady)
                return;

            double alpha = e.NewValue / 100.0;
            _overlay.AlphaValue = alpha;
            AlphaLabel.Text = string.Format(_resourceManager.GetString("AlphaLabel_Format") ?? "Onion transparency: {0}%", (int)e.NewValue);
            RefreshOnionSkinPreview();
        }

        private void ReferenceAlphaSlider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (!_uiReady)
                return;

            double alpha = e.NewValue / 100.0;
            _overlay.ReferenceAlphaValue = alpha;
            ReferenceAlphaLabel.Text = string.Format(_resourceManager.GetString("ReferenceAlphaLabel_Format") ?? "Reference transparency: {0}%", (int)e.NewValue);
            RefreshReferenceOverlayPreview();
        }

        private void LoopOverlayAlphaSlider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (!_uiReady)
                return;

            double alpha = e.NewValue / 100.0;
            _overlay.LoopPlaybackAlphaValue = alpha;
            LoopOverlayAlphaLabel.Text = string.Format(_resourceManager.GetString("LoopOverlayAlphaLabel_Format") ?? "Loop overlay: {0}%", (int)e.NewValue);
            RefreshLoopPlaybackOverlay();
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
            SetStatus(string.Format(_resourceManager.GetString("GridOverlayStatus_Format") ?? "Grid overlay: {0}", _overlay.ShowGrid ? _resourceManager.GetString("ToggleOnLabel") ?? "ON ✓" : _resourceManager.GetString("ToggleOffLabel") ?? "OFF"));
        }

        private void ActionSafeToggle_IsCheckedChanged(object sender, RoutedEventArgs e)
        {
            if (!_uiReady)
                return;

            _overlay.ShowActionSafe = ActionSafeToggle.IsChecked == true;
            RefreshOverlayCanvas();
            SetStatus(string.Format(_resourceManager.GetString("ActionSafeZoneStatus_Format") ?? "Action safe zone: {0}", _overlay.ShowActionSafe ? _resourceManager.GetString("ToggleOnLabel") ?? "ON ✓" : _resourceManager.GetString("ToggleOffLabel") ?? "OFF"));
        }

        private void TitleSafeToggle_IsCheckedChanged(object sender, RoutedEventArgs e)
        {
            if (!_uiReady)
                return;

            _overlay.ShowTitleSafe = TitleSafeToggle.IsChecked == true;
            RefreshOverlayCanvas();
            SetStatus(string.Format(_resourceManager.GetString("TitleSafeZoneStatus_Format") ?? "Title safe zone: {0}", _overlay.ShowTitleSafe ? _resourceManager.GetString("ToggleOnLabel") ?? "ON ✓" : _resourceManager.GetString("ToggleOffLabel") ?? "OFF"));
        }

        private void RefreshOnionSkinPreview()
        {
            if (!_overlay.IsEnabled)
            {
                ClearOnionSkinLayers();
                ClearTintedOnionCache();
                RefreshLoopPlaybackOverlay();
                return;
            }

            var recentFrames = GetOnionFramesForCurrentCursor(Math.Min(_overlay.OnionLayers, _onionSkinLayers.Length));
            HashSet<string> usedTintedKeys = new();
            for (int i = 0; i < _onionSkinLayers.Length; i++)
            {
                if (i < recentFrames.Count)
                {
                    Bitmap? source = recentFrames[i].PreviewFrame;
                    if (_overlay.ColorCodedMode)
                    {
                        source = GetOrCreateTintedOnionFrame(recentFrames[i], i + 1, usedTintedKeys);
                    }

                    _onionSkinLayers[i].Source = source;
                    _onionSkinLayers[i].Opacity = _overlay.GetAlphaForLayer(i + 1);
                    _onionSkinLayers[i].IsVisible = true;
                }
                else
                {
                    _onionSkinLayers[i].Source = null;
                    _onionSkinLayers[i].IsVisible = false;
                }
            }

            if (_overlay.ColorCodedMode)
                PruneTintedOnionCache(usedTintedKeys);
            else
                ClearTintedOnionCache();

            RefreshLoopPlaybackOverlay();
        }

        private void RefreshLoopPlaybackOverlay()
        {
            if (!_overlay.ShowLoopPlaybackOverlay || _capture.Frames.Count < 2)
            {
                LoopPlaybackOverlayImage.Source = null;
                LoopPlaybackOverlayImage.IsVisible = false;
                return;
            }

            CapturedFrame first = _capture.Frames[0];
            CapturedFrame last = _capture.Frames[^1];
            int currentFrame = Math.Max(1, _timelineCursorFrame);
            int frameCount = _capture.Frames.Count;

            // Blend only near loop boundaries: beginning blends against end, end against beginning.
            int blendWindow = Math.Clamp(Math.Min(8, frameCount / 2), 1, 8);

            Bitmap? compareFrame = null;
            double blendFactor = 0.0;

            int fromStart = currentFrame - first.Index;
            if (fromStart >= 0 && fromStart < blendWindow)
            {
                compareFrame = last.PreviewFrame;
                blendFactor = 1.0 - (fromStart / (double)blendWindow);
            }
            else
            {
                int fromEnd = last.Index - currentFrame;
                if (fromEnd >= 0 && fromEnd < blendWindow)
                {
                compareFrame = first.PreviewFrame;
                    blendFactor = 1.0 - (fromEnd / (double)blendWindow);
                }
            }

            if (compareFrame is null || blendFactor <= 0.0)
            {
                LoopPlaybackOverlayImage.Source = null;
                LoopPlaybackOverlayImage.IsVisible = false;
                return;
            }

            LoopPlaybackOverlayImage.Source = compareFrame;
            LoopPlaybackOverlayImage.Opacity = _overlay.LoopPlaybackAlphaValue * blendFactor;
            LoopPlaybackOverlayImage.IsVisible = true;
        }

        private IReadOnlyList<CapturedFrame> GetOnionFramesForCurrentCursor(int maxCount)
        {
            if (maxCount <= 0 || _capture.Frames.Count == 0)
                return Array.Empty<CapturedFrame>();

            int cursorFrame = Math.Max(1, _timelineCursorFrame);
            bool includeCurrentFrame = _camera.IsRunning && _playbackIndex < 0;
            List<CapturedFrame> result = new(maxCount);

            for (int i = _capture.Frames.Count - 1; i >= 0 && result.Count < maxCount; i--)
            {
                CapturedFrame frame = _capture.Frames[i];
                bool isUsable = includeCurrentFrame
                    ? frame.Index <= cursorFrame
                    : frame.Index < cursorFrame;

                if (isUsable)
                    result.Add(frame);
            }

            return result;
        }

        private void ClearOnionSkinLayers()
        {
            foreach (var layer in _onionSkinLayers)
            {
                layer.Source = null;
                layer.IsVisible = false;
            }
        }

        private Bitmap? GetOrCreateTintedOnionFrame(CapturedFrame frame, int layerIndex, HashSet<string> usedKeys)
        {
            if (frame.PreviewFrame == null)
                return null;

            int tintIndex = Math.Clamp(layerIndex - 1, 0, OnionTintPalette.Length - 1);
            Color tint = OnionTintPalette[tintIndex];
            string key = $"{RuntimeHelpers.GetHashCode(frame.PreviewFrame)}:{layerIndex}";
            usedKeys.Add(key);

            if (_tintedOnionCache.TryGetValue(key, out Bitmap? cached))
                return cached;

            Bitmap tinted = CreateTintedBitmap(frame.PreviewFrame, tint);
            _tintedOnionCache[key] = tinted;
            return tinted;
        }

        private static unsafe Bitmap CreateTintedBitmap(Bitmap source, Color tint)
        {
            int width = source.PixelSize.Width;
            int height = source.PixelSize.Height;
            if (width <= 0 || height <= 0)
                return source;

            int stride = width * 4;
            int bufferSize = stride * height;
            byte[] sourceBuffer = new byte[bufferSize];

            fixed (byte* srcPtr = sourceBuffer)
            {
                source.CopyPixels(new PixelRect(0, 0, width, height), (IntPtr)srcPtr, bufferSize, stride);
            }

            var tintedBitmap = new WriteableBitmap(new PixelSize(width, height), new Vector(96, 96), PixelFormat.Bgra8888, AlphaFormat.Unpremul);
            using (var framebuffer = tintedBitmap.Lock())
            {
                byte* dest = (byte*)framebuffer.Address.ToPointer();
                int rowBytes = framebuffer.RowBytes;

                Parallel.For(0, height, y =>
                {
                    int srcRow = y * stride;
                    int dstRow = y * rowBytes;
                    for (int x = 0; x < width; x++)
                    {
                        int srcOffset = srcRow + x * 4;
                        int dstOffset = dstRow + x * 4;

                        byte b = sourceBuffer[srcOffset];
                        byte g = sourceBuffer[srcOffset + 1];
                        byte r = sourceBuffer[srcOffset + 2];
                        byte a = sourceBuffer[srcOffset + 3];

                        double luminance = ((0.299 * r) + (0.587 * g) + (0.114 * b)) / 255.0;

                        dest[dstOffset] = (byte)Math.Clamp(tint.B * luminance, 0, 255);
                        dest[dstOffset + 1] = (byte)Math.Clamp(tint.G * luminance, 0, 255);
                        dest[dstOffset + 2] = (byte)Math.Clamp(tint.R * luminance, 0, 255);
                        dest[dstOffset + 3] = a;
                    }
                });
            }

            return tintedBitmap;
        }

        private void PruneTintedOnionCache(HashSet<string> usedKeys)
        {
            List<string> keysToRemove = new();
            foreach (string key in _tintedOnionCache.Keys)
            {
                if (!usedKeys.Contains(key))
                    keysToRemove.Add(key);
            }

            foreach (string key in keysToRemove)
            {
                if (_tintedOnionCache.Remove(key, out Bitmap? bitmap))
                    bitmap.Dispose();
            }
        }

        private void ClearTintedOnionCache()
        {
            foreach (Bitmap bitmap in _tintedOnionCache.Values)
                bitmap.Dispose();

            _tintedOnionCache.Clear();
        }

        private void RefreshReferenceOverlayPreview()
        {
            if (ReferenceOverlayToggle.IsChecked != true || !_referenceOverlay.HasSource)
            {
                ReferenceOverlayImage.Source = null;
                ReferenceOverlayImage.IsVisible = false;
                return;
            }

            try
            {
                int frameNumber = Math.Max(1, _timelineCursorFrame);
                Bitmap? frame = _referenceOverlay.GetFrameForTimelineFrame(frameNumber);
                if (frame == null)
                {
                    ReferenceOverlayImage.Source = null;
                    ReferenceOverlayImage.IsVisible = false;
                    return;
                }

                ReferenceOverlayImage.Source = frame;
                ReferenceOverlayImage.Opacity = _overlay.ReferenceAlphaValue;
                ReferenceOverlayImage.IsVisible = true;
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("ReferenceOverlay", $"Refresh failed: {ex.Message}");
                ReferenceOverlayImage.Source = null;
                ReferenceOverlayImage.IsVisible = false;
            }
        }

        private async void LoadReferenceOverlayFile_Click(object? sender, RoutedEventArgs e)
        {
            try
            {
                IReadOnlyList<IStorageFile> files = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
                {
                    Title = _resourceManager.GetString("OverlayPicker_Title") ?? "Choose overlay video/image",
                    AllowMultiple = false,
                    FileTypeFilter = new[]
                    {
                        new FilePickerFileType("Overlay Medien")
                        {
                            Patterns = new[] { "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.gif", "*.webp", "*.mp4", "*.mov", "*.avi", "*.mkv", "*.wmv", "*.m4v" }
                        }
                    }
                });

                if (files.Count == 0)
                    return;

                string filePath = files[0].Path.LocalPath;
                string extension = Path.GetExtension(filePath).ToLowerInvariant();
                bool isVideo = extension is ".mp4" or ".mov" or ".avi" or ".mkv" or ".wmv" or ".m4v";

                if (isVideo)
                    _referenceOverlay.LoadVideo(filePath);
                else if (ReferenceOverlayProvider.IsSupportedImageFile(filePath))
                    _referenceOverlay.LoadSingleImage(filePath);
                else
                    throw new InvalidOperationException(_resourceManager.GetString("OverlayUnsupportedFileType") ?? "The file type is not supported for overlays.");

                ReferenceOverlayInfoText.Text = _referenceOverlay.SourceLabel;
                ReferenceOverlayToggle.IsChecked = true;
                RefreshReferenceOverlayPreview();
                SetStatus(string.Format(_resourceManager.GetString("ReferenceOverlay_LoadedMessage") ?? "Reference loaded: {0}", _referenceOverlay.SourceLabel));
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("ReferenceOverlay", $"Load file failed: {ex.Message}");
                await MessageBox.Show(this, _resourceManager.GetString("ReferenceOverlay_Title") ?? "Reference overlay", string.Format(_resourceManager.GetString("ReferenceOverlay_LoadFailedMessage") ?? "Overlay could not be loaded:\n{0}", ex.Message));
            }
        }

        private async void LoadReferenceImageSequence_Click(object? sender, RoutedEventArgs e)
        {
            try
            {
                string? folder = await BrowseForFolder(_resourceManager.GetString("ReferenceOverlay_SelectSequenceTitle") ?? "Choose overlay image sequence");
                if (string.IsNullOrWhiteSpace(folder))
                    return;

                _referenceOverlay.LoadImageSequence(folder);
                ReferenceOverlayInfoText.Text = _referenceOverlay.SourceLabel;
                ReferenceOverlayToggle.IsChecked = true;
                RefreshReferenceOverlayPreview();
                SetStatus(string.Format(_resourceManager.GetString("ReferenceOverlay_LoadedMessage") ?? "Reference loaded: {0}", _referenceOverlay.SourceLabel));
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("ReferenceOverlay", $"Load sequence failed: {ex.Message}");
                await MessageBox.Show(this, _resourceManager.GetString("ReferenceOverlay_Title") ?? "Reference overlay", string.Format(_resourceManager.GetString("ReferenceOverlay_SequenceLoadFailedMessage") ?? "Image sequence could not be loaded:\n{0}", ex.Message));
            }
        }

        private void ClearReferenceOverlay_Click(object? sender, RoutedEventArgs e)
        {
            _referenceOverlay.Clear();
            ReferenceOverlayInfoText.Text = _referenceOverlay.SourceLabel;
            ReferenceOverlayToggle.IsChecked = false;
            ReferenceOverlayImage.Source = null;
            ReferenceOverlayImage.IsVisible = false;
            SetStatus(_resourceManager.GetString("ReferenceOverlay_RemovedMessage") ?? "Reference overlay removed.");
        }

        private void ReferencePlaybackModeButton_Click(object? sender, RoutedEventArgs e)
        {
            _referenceOverlay.PlaybackMode = _referenceOverlay.PlaybackMode == ReferenceOverlayPlaybackMode.Loop
                ? ReferenceOverlayPlaybackMode.HoldLastFrame
                : ReferenceOverlayPlaybackMode.Loop;

            UpdateReferencePlaybackModeButton();
            RefreshReferenceOverlayPreview();

            SetStatus(_referenceOverlay.PlaybackMode == ReferenceOverlayPlaybackMode.Loop
                ? _resourceManager.GetString("ReferencePlaybackMode_Loop") ?? "Reference mode: loop active."
                : _resourceManager.GetString("ReferencePlaybackMode_StopAtEnd") ?? "Reference mode: stops on the last frame.");
        }

        private void UpdateReferencePlaybackModeButton()
        {
            _referenceLoopIcon ??= LoadReferencePlaybackIcon("reference_mode_loop");
            _referenceHoldIcon ??= LoadReferencePlaybackIcon("reference_mode_hold");

            bool isLoop = _referenceOverlay.PlaybackMode == ReferenceOverlayPlaybackMode.Loop;
            ReferencePlaybackModeText.Text = isLoop ? _resourceManager.GetString("ReferencePlaybackMode_LabelLoop") ?? "Mode: Loop" : _resourceManager.GetString("ReferencePlaybackMode_LabelEnd") ?? "Mode: Until end";
            ReferencePlaybackModeIcon.Source = isLoop ? _referenceLoopIcon : _referenceHoldIcon;
        }

        private void UpdateTabIcons()
        {
            _tabCaptureIcon ??= LoadReferencePlaybackIcon("tab_capture");
            _tabOnionOverlayIcon ??= LoadReferencePlaybackIcon("tab_onion_overlay");
            _tabProjectRawIcon ??= LoadReferencePlaybackIcon("tab_project_raw");
            _tabHardwareIcon ??= LoadReferencePlaybackIcon("tab_hardware");

            TabIconCapture.Source = _tabCaptureIcon;
            TabIconOnionOverlay.Source = _tabOnionOverlayIcon;
            TabIconProjectRaw.Source = _tabProjectRawIcon;
            TabIconHardware.Source = _tabHardwareIcon;
        }

        private Bitmap? LoadReferencePlaybackIcon(string baseFileName)
        {
            string preferredSize = RenderScaling >= 1.5 ? "48" : "24";
            string fallbackSize = preferredSize == "48" ? "24" : "48";

            Bitmap? preferred = TryLoadReferencePlaybackIcon(baseFileName, preferredSize);
            return preferred ?? TryLoadReferencePlaybackIcon(baseFileName, fallbackSize);
        }

        private Bitmap? TryLoadReferencePlaybackIcon(string baseFileName, string size)
        {
            try
            {
                var uri = new Uri($"avares://OpenStopMotionStudio/Assets/icons/reference-overlay/{baseFileName}_{size}.png");
                using Stream stream = AssetLoader.Open(uri);
                return new Bitmap(stream);
            }
            catch
            {
                return null;
            }
        }

        private void ReferenceOverlayToggle_IsCheckedChanged(object? sender, RoutedEventArgs e)
        {
            bool isEnabled = ReferenceOverlayToggle.IsChecked == true;
            ReferenceOverlayToggle.Content = isEnabled ? _resourceManager.GetString("ReferenceOverlayToggle_On") ?? "Reference overlay: ON ✓" : _resourceManager.GetString("ReferenceOverlayToggle_Off") ?? "Reference overlay: OFF";

            if (isEnabled)
                RefreshReferenceOverlayPreview();
            else
            {
                ReferenceOverlayImage.Source = null;
                ReferenceOverlayImage.IsVisible = false;
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
            SetStatus(string.Format(_resourceManager.GetString("PreviousKeyframeMessage") ?? "Go to previous keyframe: Frame {0}", _timelineCursorFrame));
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
            SetStatus(string.Format(_resourceManager.GetString("NextKeyframeMessage") ?? "Go to next keyframe: Frame {0}", _timelineCursorFrame));
        }

        private void PlayPause_Click(object sender, RoutedEventArgs e)
        {
            if (_capture.Frames.Count == 0)
                return;

            if (_playbackTimer.IsEnabled)
            {
                StopPlaybackInternal();
                RefreshTimelineState();
                SetStatus(_resourceManager.GetString("PlaybackPausedMessage") ?? "Playback paused.");
                return;
            }

            if (_playbackIndex < 0)
                _playbackIndex = GetNearestCaptureIndexAtOrAfter(_timelineCursorFrame);

            ShowFrameAtPlaybackIndex(_playbackIndex);
            _playbackTimer.Start();
            PlayPauseButton.Content = _resourceManager.GetString("PauseButton_Content") ?? "⏸ Pause";
            RefreshTimelineState();
            SetStatus(string.Format(_resourceManager.GetString("PlaybackStartedMessage") ?? "Playback started at {0} fps.", _playbackFps));
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
            RefreshReferenceOverlayPreview();
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

            PlayPauseButton.Content = _resourceManager.GetString("PlayPauseButton_Content_Play") ?? "▶ Play";
        }

        private async void ConnectStreamDeck_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                bool connected = _streamDeck.Connect();

                if (connected)
                {
                    StreamDeckStatusText.Text = _resourceManager.GetString("StreamDeck_Status_Connected") ?? "● Connected";
                    StreamDeckStatusText.Foreground = SolidColorBrush.Parse("#4CAF50");
                    SetStatus(_resourceManager.GetString("StreamDeck_ConnectedMessage") ?? "Stream Deck connected and ready.");
                }
                else
                {
                    StreamDeckStatusText.Text = _resourceManager.GetString("StreamDeck_Status_NotFound") ?? "● Not found";
                    await MessageBox.Show(this, _resourceManager.GetString("StreamDeck_Title") ?? "Stream Deck", _resourceManager.GetString("StreamDeck_NotFoundMessage") ?? "No Stream Deck found.\nPlease connect the device and try again.");
                }
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("ConnectStreamDeck_Click", $"Error connecting to Stream Deck: {ex.Message}");
                StreamDeckStatusText.Text = _resourceManager.GetString("StreamDeck_Status_Error") ?? "● Error";
                await MessageBox.Show(this, _resourceManager.GetString("StreamDeck_Title") ?? "Stream Deck", string.Format(_resourceManager.GetString("StreamDeck_ConnectionErrorMessage") ?? "Error connecting to the Stream Deck: {0}", ex.Message));
            }
        }

        private async void SelectFolder_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string? folder = await BrowseForFolder(_resourceManager.GetString("SelectProjectFolder_Title") ?? "Select project folder");
                if (string.IsNullOrWhiteSpace(folder))
                    return;

                _capture.SetOutputFolder(folder);
                ProjectFolderText.Text = folder;
                UpdateShotPreview();
                SetStatus(string.Format(_resourceManager.GetString("ProjectFolderStatus_Format") ?? "Project folder: {0}", folder));
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("SelectFolder_Click", $"Error selecting folder: {ex.Message}");
                await MessageBox.Show(this, _resourceManager.GetString("Error_Title") ?? "Error", string.Format(_resourceManager.GetString("FolderSelectionErrorMessage") ?? "Error selecting folder: {0}", ex.Message));
            }
        }

        private void CancelNefImportButton_Click(object? sender, RoutedEventArgs e)
        {
            CancelNefImportRequested();
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
                ? _resourceManager.GetString("CaptureFormat_PngWithProxy") ?? "Capture format: PNG master + JPEG proxies"
                : _resourceManager.GetString("CaptureFormat_JpegMaster") ?? "Capture format: JPEG master");
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
            RefreshReferenceOverlayPreview();
            UpdateShotPreview();
            RefreshTimelineState();
            SetStatus(string.Format(_resourceManager.GetString("ShotActiveMessage_Format") ?? "Shot active: {0}", shotName));
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

        private CameraDeviceDescriptor? GetSelectedCameraDescriptor()
        {
            int selectedIndex = CameraComboBox.SelectedIndex;
            return selectedIndex >= 0 && selectedIndex < _cameraDevices.Count
                ? _cameraDevices[selectedIndex]
                : null;
        }

        private static bool IsSdkBackedCamera(CameraDeviceDescriptor? descriptor)
        {
            return descriptor is not null && descriptor.ConnectionKind != CameraConnectionKind.GenericVideo;
        }

        private static string DescribeConnectionKind(CameraConnectionKind kind)
        {
            return kind switch
            {
                CameraConnectionKind.CanonEdsdk => "Canon EDSDK",
                CameraConnectionKind.NikonMaid => "Nikon MAID",
                CameraConnectionKind.SonyCr => "Sony CrSDK",
                _ => "Generisches Video-Backend"
            };
        }

        private void UpdateCameraIntegrationStatus(string? statusOverride = null)
        {
            CameraDeviceDescriptor? descriptor = GetSelectedCameraDescriptor();
            CameraConnectionKind backendKind = _camera.CurrentConnectionKind ?? descriptor?.ConnectionKind ?? CameraConnectionKind.GenericVideo;

            if (descriptor is null)
            {
                CameraBackendStatusText.Text = _resourceManager.GetString("CameraStatus_NoCameraSelected_Backend") ?? "Backend: no camera selected";
                CameraCaptureModeText.Text = _resourceManager.GetString("CameraStatus_NoCameraSelected_Mode") ?? "Capture mode: inactive";
                CameraSdkStatusText.Text = _resourceManager.GetString("CameraStatus_NoCameraSelected_Sdk") ?? "SDK status: no camera selected";
                return;
            }

            CameraBackendStatusText.Text = string.Format(_resourceManager.GetString("CameraStatus_BackendFormat") ?? "Backend: {0}", DescribeConnectionKind(backendKind));

            if (_camera.IsRunning)
            {
                CameraCaptureModeText.Text = _camera.UsesHardwareStillCapture
                    ? _resourceManager.GetString("CameraStatus_CaptureMode_Dslr") ?? "Capture mode: DSLR still capture via manufacturer SDK"
                    : _resourceManager.GetString("CameraStatus_CaptureMode_Live") ?? "Capture mode: live frame capture via video backend";
            }
            else
            {
                CameraCaptureModeText.Text = IsSdkBackedCamera(descriptor)
                    ? _resourceManager.GetString("CameraStatus_CaptureMode_SdkSelected") ?? "Capture mode: SDK camera selected, connection not started yet"
                    : _resourceManager.GetString("CameraStatus_CaptureMode_GenericVideo") ?? "Capture mode: generic video source selected";
            }

            string baseStatus = IsSdkBackedCamera(descriptor)
                ? string.Format(_resourceManager.GetString("CameraStatus_SdkDetectedFormat") ?? "SDK status: {0} for {1} detected", descriptor.AdapterName, descriptor.Name)
                : string.Format(_resourceManager.GetString("CameraStatus_SdkNotUsedFormat") ?? "SDK status: {0} uses no manufacturer SDK", descriptor.Name);

            CameraSdkStatusText.Text = string.IsNullOrWhiteSpace(statusOverride)
                ? baseStatus
                : string.Format(_resourceManager.GetString("CameraStatus_LastStatusFormat") ?? "{0}\nLast status: {1}", baseStatus, statusOverride);
        }

        private void OnCameraStatusChanged(string message)
        {
            Dispatcher.UIThread.Post(() =>
            {
                UpdateCameraIntegrationStatus(message);
                SetStatus(message);
                DebugLogger.Instance.LogInfo("Camera", message);
            });
        }

        private void UpdateFrameCounterText()
        {
            FrameCounterText.Text = _capture.FrameCount == 0
                ? _resourceManager.GetString("FrameCounterText_Zero") ?? "Frame: 0"
                : string.Format(_resourceManager.GetString("FrameCounterText_Value") ?? "Frame: {0}", _capture.LastFrameNumber);
        }

        private void UpdatePlaybackSpeedLabel()
        {
            PlaybackSpeedLabel.Text = string.Format(_resourceManager.GetString("PlaybackSpeedLabel_Format") ?? "Playback: {0} fps", _playbackFps);
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
            _referenceOverlay.Dispose();
            ClearTintedOnionCache();
            _referenceLoopIcon?.Dispose();
            _referenceHoldIcon?.Dispose();
            _tabCaptureIcon?.Dispose();
            _tabOnionOverlayIcon?.Dispose();
            _tabProjectRawIcon?.Dispose();
            _tabHardwareIcon?.Dispose();
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
                SetStatus(string.Format(_resourceManager.GetString("ProjectLoadingMessage") ?? "Loading project: {0}...", projectPath));
                
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
                        RefreshReferenceOverlayPreview();
                        RefreshTimelineState();
                        EnsureTimelineCursorVisible();
                        ShowFrameAtPlaybackIndex(loadSummary.LoadedFrameCount - 1);

                        string migrationText = migration.HasChanges
                            ? string.Format(_resourceManager.GetString("ProjectLoadedMigrationText") ?? " | Migration: {0} files", migration.MovedFiles)
                            : string.Empty;
                        SetStatus(string.Format(_resourceManager.GetString("ProjectLoadedMessage") ?? "Project loaded: {0} | Shot {1} with {2} frames{3}", projectPath, loadSummary.ShotName, loadSummary.LoadedFrameCount, migrationText));
                    }
                    else
                    {
                        UpdateShotPreview();
                        RefreshTimelineState();
                        if (migration.HasChanges)
                        {
                            SetStatus(string.Format(_resourceManager.GetString("ProjectLoadedMigrationCompletedMessage") ?? "Project loaded: {0} | Migration completed: {1} files moved", projectPath, migration.MovedFiles));
                        }
                        else
                        {
                            SetStatus(string.Format(_resourceManager.GetString("ProjectLoadedSimpleMessage") ?? "Project loaded: {0}", projectPath));
                        }
                    }
                });
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("ProjectLoad", $"Failed to load project '{projectPath}'. Reason: {ex.Message}");
                await Dispatcher.UIThread.InvokeAsync(async () =>
                {
                    await MessageBox.Show(this, _resourceManager.GetString("ProjectLoadError_Title") ?? "Error loading project", string.Format(_resourceManager.GetString("ProjectLoadError_Message") ?? "The project '{0}' could not be loaded.\n\nReason: {1}", Path.GetFileName(projectPath), ex.Message));
                    
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
