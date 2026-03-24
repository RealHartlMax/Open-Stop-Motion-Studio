using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using Microsoft.Win32;
using OpenStopMotionStudio.Core;

namespace OpenStopMotionStudio.GUI
{
    /// <summary>
    /// MainWindow: Koordiniert alle Manager-Klassen und verbindet
    /// die UI-Events mit der Business-Logik im Core Layer.
    /// Das Fenster selbst enthält keine Logik – es delegiert alles.
    /// </summary>
    public partial class MainWindow : Window
    {
        // ── Core Manager Instanzen ──────────────────────────────────────────
        private readonly CameraManager     _camera      = new();
        private readonly CaptureManager   _capture     = new();
        private readonly OverlayManager   _overlay     = new();
        private readonly StreamDeckManager _streamDeck  = new();

        public MainWindow()
        {
            InitializeComponent();
            BootstrapApplication();
        }

        // ════════════════════════════════════════════════════════════════════
        //  INITIALISIERUNG
        // ════════════════════════════════════════════════════════════════════

        /// <summary>
        /// Startet die Anwendung: Kameras erkennen, Stream Deck prüfen,
        /// Keyboard-Handler registrieren, Events verdrahten.
        /// </summary>
        private void BootstrapApplication()
        {
            // Kamera-Event: neuer Frame vom CameraManager
            _camera.FrameReady += OnFrameReady;

            // Stream Deck Events: Hardware-Buttons auf Studio-Aktionen mappen
            _streamDeck.CapturePressed      += () => Dispatcher.Invoke(TriggerCapture);
            _streamDeck.OnionTogglePressed  += () => Dispatcher.Invoke(ToggleOnionSkin);
            _streamDeck.AlphaPresetPressed  += (value) => Dispatcher.Invoke(() => SetAlphaPreset(value));

            // Verfügbare Kameras in ComboBox laden
            RefreshCameraList();

            // Globaler Keyboard-Handler: Leertaste = Frame aufnehmen
            KeyDown += MainWindow_KeyDown;

            SetStatus("Bereit. Kamera auswählen und starten.");
        }

        /// <summary>
        /// Scannt nach verfügbaren DirectShow-Kameras und befüllt die ComboBox.
        /// Wird auch bei USB-Hotplug erneut aufgerufen (optional erweiterbar).
        /// </summary>
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

        // ════════════════════════════════════════════════════════════════════
        //  KAMERA EVENTS
        // ════════════════════════════════════════════════════════════════════

        private void CameraComboBox_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            // Kamera-Auswahl geändert → laufende Kamera stoppen
            if (_camera.IsRunning)
                _camera.Stop();
        }

        private void StartCameraButton_Click(object sender, RoutedEventArgs e)
        {
            if (_camera.IsRunning)
            {
                // Kamera stoppen
                _camera.Stop();
                StartCameraButton.Content = "▶ Kamera starten";
                CaptureButton.IsEnabled   = false;
                NoCameraText.Visibility   = Visibility.Visible;
                SetStatus("Kamera gestoppt.");
                return;
            }

            int selectedIndex = CameraComboBox.SelectedIndex;
            if (selectedIndex < 0) return;

            bool started = _camera.Start(selectedIndex);
            if (started)
            {
                StartCameraButton.Content = "⏹ Kamera stoppen";
                CaptureButton.IsEnabled   = true;
                NoCameraText.Visibility   = Visibility.Collapsed;
                SetStatus($"Kamera aktiv: {CameraComboBox.SelectedItem}");
            }
            else
            {
                MessageBox.Show("Kamera konnte nicht gestartet werden.", "Fehler",
                    MessageBoxButton.OK, MessageBoxImage.Warning);
            }
        }

        /// <summary>
        /// Wird für jeden neuen Frame vom CameraManager aufgerufen (~30fps).
        /// Das Overlay-Compositing passiert hier: Live-Feed + Onion Skin.
        /// </summary>
        private void OnFrameReady(BitmapSource liveFrame)
        {
            // UI-Thread: WPF erlaubt Image-Updates nur vom Dispatcher-Thread
            Dispatcher.Invoke(() =>
            {
                // 1. Live-Kamera-Bild direkt anzeigen
                LiveFeedImage.Source = liveFrame;

                // 2. Onion Skin: letztes Frame als Overlay, wenn aktiviert
                if (OnionSkinToggle.IsChecked == true && _capture.LastFrame != null)
                {
                    OnionSkinImage.Source  = _capture.LastFrame;
                    OnionSkinImage.Opacity = _overlay.AlphaValue;
                }
            });
        }

        // ════════════════════════════════════════════════════════════════════
        //  FRAME CAPTURE
        // ════════════════════════════════════════════════════════════════════

        private void CaptureButton_Click(object sender, RoutedEventArgs e) => TriggerCapture();

        private void MainWindow_KeyDown(object sender, KeyEventArgs e)
        {
            // Leertaste als globaler Capture-Shortcut (wie in professionellen Tools)
            if (e.Key == Key.Space && CaptureButton.IsEnabled)
                TriggerCapture();
        }

        /// <summary>
        /// Kernfunktion: Aktuellen Live-Frame von der Kamera holen und speichern.
        /// Nach dem Speichern wird der Frame auch als neues Onion Skin referenziert.
        /// </summary>
        private void TriggerCapture()
        {
            var currentFrame = _camera.GetCurrentFrame();
            if (currentFrame == null)
            {
                SetStatus("⚠ Kein Frame verfügbar.");
                return;
            }

            // CaptureManager speichert das Bild und aktualisiert den internen Zustand
            string savedPath = _capture.SaveFrame(currentFrame);

            // Frame-Zähler im UI aktualisieren
            FrameCounterText.Text = $"Frame: {_capture.FrameCount}";

            SetStatus($"✔ Frame {_capture.FrameCount} gespeichert: {System.IO.Path.GetFileName(savedPath)}");
        }

        // ════════════════════════════════════════════════════════════════════
        //  ONION SKIN CONTROLS
        // ════════════════════════════════════════════════════════════════════

        private void OnionSkinToggle_Checked(object sender, RoutedEventArgs e)
        {
            _overlay.IsEnabled             = true;
            OnionSkinToggle.Content        = "Onion Skin: AN ✓";
            OnionSkinImage.Visibility      = Visibility.Visible;
        }

        private void OnionSkinToggle_Unchecked(object sender, RoutedEventArgs e)
        {
            _overlay.IsEnabled             = false;
            OnionSkinToggle.Content        = "Onion Skin: AUS";
            OnionSkinImage.Visibility      = Visibility.Collapsed;
        }

        private void ToggleOnionSkin()
        {
            // Ermöglicht Toggle auch via Stream Deck (von außen aufrufbar)
            OnionSkinToggle.IsChecked = !OnionSkinToggle.IsChecked;
        }

        private void AlphaSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            double alpha = e.NewValue / 100.0; // Slider: 0–100 → Alpha: 0.0–1.0
            _overlay.AlphaValue       = alpha;
            OnionSkinImage.Opacity    = alpha;
            AlphaLabel.Text           = $"Transparenz: {(int)e.NewValue}%";
        }

        private void AlphaPreset_Click(object sender, RoutedEventArgs e)
        {
            // Tag-Attribut des Buttons enthält den Prozentwert als String
            if (sender is System.Windows.Controls.Button btn &&
                int.TryParse(btn.Tag?.ToString(), out int preset))
                SetAlphaPreset(preset);
        }

        private void SetAlphaPreset(int percent)
        {
            AlphaSlider.Value = percent; // löst AlphaSlider_ValueChanged aus
        }

        // ════════════════════════════════════════════════════════════════════
        //  STREAM DECK
        // ════════════════════════════════════════════════════════════════════

        private void ConnectStreamDeck_Click(object sender, RoutedEventArgs e)
        {
            bool connected = _streamDeck.Connect();

            if (connected)
            {
                StreamDeckStatusText.Text       = "● Verbunden";
                StreamDeckStatusText.Foreground = new System.Windows.Media.SolidColorBrush(
                    System.Windows.Media.Color.FromRgb(0x4C, 0xAF, 0x50)); // Grün
                SetStatus("Stream Deck verbunden und bereit.");
            }
            else
            {
                StreamDeckStatusText.Text = "● Nicht gefunden";
                MessageBox.Show(
                    "Kein Stream Deck gefunden.\nBitte Gerät anschließen und erneut versuchen.",
                    "Stream Deck", MessageBoxButton.OK, MessageBoxImage.Information);
            }
        }

        // ════════════════════════════════════════════════════════════════════
        //  PROJEKT-ORDNER
        // ════════════════════════════════════════════════════════════════════

        private void SelectFolder_Click(object sender, RoutedEventArgs e)
        {
            // WPF hat keinen FolderBrowserDialog → OpenFileDialog als Workaround
            var dialog = new OpenFileDialog
            {
                Title            = "Projektordner auswählen",
                CheckFileExists  = false,
                CheckPathExists  = true,
                FileName         = "Ordner auswählen",
                Filter           = "Ordner|*.none",
                ValidateNames    = false
            };

            if (dialog.ShowDialog() == true)
            {
                string folder = System.IO.Path.GetDirectoryName(dialog.FileName)!;
                _capture.SetOutputFolder(folder);
                ProjectFolderText.Text = folder;
                SetStatus($"Projektordner: {folder}");
            }
        }

        // ════════════════════════════════════════════════════════════════════
        //  HILFSMETHODEN
        // ════════════════════════════════════════════════════════════════════

        private void SetStatus(string message)
        {
            StatusBarText.Text = message;
        }

        protected override void OnClosed(EventArgs e)
        {
            // Sauber aufräumen: Kamera freigeben, Stream Deck trennen
            _camera.Stop();
            _camera.Dispose();
            _streamDeck.Disconnect();
            base.OnClosed(e);
        }
    }
}
