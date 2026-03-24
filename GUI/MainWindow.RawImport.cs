using System;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using OpenStopMotionStudio.Core;

namespace OpenStopMotionStudio.GUI
{
    public partial class MainWindow
    {
        private const int DefaultRawImportFrameStart = 1001;

        private readonly NikonSdkDiscovery _nikonSdkDiscovery = new();
        private readonly NikonNefImportService _nikonNefImportService = new();

        private NikonImageSdkLocation? _nikonImageSdkLocation;
        private string? _rawSourceFolder;
        private bool _rawImportBusy;

        private void InitializeRawImportUi()
        {
            RawProxyFormatComboBox.SelectedIndex = 0;
            RawFrameStartTextBox.Text = DefaultRawImportFrameStart.ToString();
            RefreshNikonSdkStatus();
            UpdateRawImportUiState();
        }

        private void RefreshNikonSdkStatus()
        {
            _nikonImageSdkLocation = _nikonSdkDiscovery.FindImageSdk();

            if (_nikonImageSdkLocation is null)
            {
                RawSdkStatusText.Text = "Nikon SDK: kein lokales NkImgSDK gefunden. Bitte SDKs/Nikon pruefen.";
                RawSdkStatusText.Foreground = new SolidColorBrush(Color.FromRgb(0xFF, 0x9A, 0x76));
                return;
            }

            RawSdkStatusText.Text = $"Nikon SDK: {_nikonImageSdkLocation.DisplayName} bereit";
            RawSdkStatusText.Foreground = new SolidColorBrush(Color.FromRgb(0x99, 0xD9, 0x8C));
        }

        private void UpdateRawImportUiState()
        {
            ImportNefButton.IsEnabled = !_rawImportBusy
                && _nikonImageSdkLocation is not null
                && !string.IsNullOrWhiteSpace(_rawSourceFolder);

            ImportNefButton.Content = _rawImportBusy ? "NEF-Import laeuft..." : "NEF importieren";
        }

        private void SelectRawSourceFolder_Click(object sender, RoutedEventArgs e)
        {
            string? folder = BrowseForFolder("RAW-Ordner auswählen");
            if (string.IsNullOrWhiteSpace(folder))
                return;

            _rawSourceFolder = folder;
            RawSourceFolderText.Text = folder;
            UpdateRawImportUiState();
            SetStatus($"RAW-Quelle: {folder}");
        }

        private async void ImportNefButton_Click(object sender, RoutedEventArgs e)
        {
            if (_rawImportBusy)
                return;

            RefreshNikonSdkStatus();
            if (_nikonImageSdkLocation is null)
            {
                MessageBox.Show(
                    "Kein passendes Nikon Image SDK gefunden.\nBitte den lokalen SDK-Ordner unter SDKs/Nikon prüfen.",
                    "NEF-Import",
                    MessageBoxButton.OK,
                    MessageBoxImage.Information);
                UpdateRawImportUiState();
                return;
            }

            if (string.IsNullOrWhiteSpace(_rawSourceFolder))
            {
                MessageBox.Show(
                    "Bitte zuerst einen RAW-Ordner mit NEF-Dateien auswählen.",
                    "NEF-Import",
                    MessageBoxButton.OK,
                    MessageBoxImage.Information);
                return;
            }

            _rawImportBusy = true;
            UpdateRawImportUiState();

            string shotName = CaptureManager.NormalizeShotName(ShotNameTextBox.Text);
            int frameStart = NormalizeRawFrameStartInput();
            ShotNameTextBox.Text = shotName;

            NefImportSettings settings = new()
            {
                ProjectFolder = _capture.OutputFolder,
                ShotName = shotName,
                FrameStart = frameStart,
                ProxyFormat = GetSelectedRawProxyFormat()
            };

            Progress<NefImportProgress> progress = new(info =>
            {
                SetStatus($"NEF-Import {info.Current}/{info.Total}: {info.FileName}");
            });

            try
            {
                StopPlaybackInternal();
                HidePlaybackPreview();
                _playbackIndex = -1;

                NefImportSummary summary = await Task.Run(() =>
                    _nikonNefImportService.ImportFolder(_rawSourceFolder!, settings, progress));

                ApplyImportedFrames(summary);
                SetStatus($"NEF-Import fertig: {summary.ImportedCount} Frames nach {summary.MasterFolder}");
            }
            catch (Exception ex)
            {
                MessageBox.Show(
                    ex.Message,
                    "NEF-Import",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error);
                SetStatus("NEF-Import fehlgeschlagen.");
            }
            finally
            {
                _rawImportBusy = false;
                UpdateRawImportUiState();
            }
        }

        private void RawFrameStartTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key != Key.Enter)
                return;

            NormalizeRawFrameStartInput();
            e.Handled = true;
        }

        private void RawFrameStartTextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            NormalizeRawFrameStartInput();
        }

        private int NormalizeRawFrameStartInput()
        {
            if (!int.TryParse(RawFrameStartTextBox.Text, out int frameStart))
                frameStart = DefaultRawImportFrameStart;

            frameStart = Math.Clamp(frameStart, 1, 999_999);
            RawFrameStartTextBox.Text = frameStart.ToString();
            return frameStart;
        }

        private RawImportProxyFormat GetSelectedRawProxyFormat()
        {
            return RawProxyFormatComboBox.SelectedIndex switch
            {
                1 => RawImportProxyFormat.Png,
                _ => RawImportProxyFormat.Jpeg
            };
        }

        private void ApplyImportedFrames(NefImportSummary summary)
        {
            _capture.LoadImportedFrames(summary.ShotName, summary.FrameStart, summary.ImportedFrames);
            ProjectFolderText.Text = _capture.OutputFolder;
            ShotNameTextBox.Text = summary.ShotName;
            _timelineCursorFrame = summary.ImportedCount > 0
                ? summary.ImportedFrames[^1].Index
                : summary.FrameStart;

            UpdateFrameCounterText();
            UpdateShotPreview();
            RefreshOnionSkinPreview();
            RefreshTimelineState();
            EnsureTimelineCursorVisible();

            if (!_camera.IsRunning && summary.ImportedCount > 0)
            {
                ShowFrameAtPlaybackIndex(summary.ImportedCount - 1);
            }
            else
            {
                _playbackIndex = -1;
                HidePlaybackPreview();
            }
        }
    }
}
