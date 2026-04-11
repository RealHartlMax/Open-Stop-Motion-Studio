using System;
using System.Threading;
using System.Threading.Tasks;
using System.Linq;
using Avalonia.Controls;
using Avalonia.Threading;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Media;
using OpenStopMotionStudio.Core;

namespace OpenStopMotionStudio.GUI
{
    public partial class MainWindow
    {
        private const int DefaultRawImportFrameStart = 1001;
        private const int ImportDialogAutoCloseSuccessMs = 900;
        private const int ImportDialogAutoCloseWarningMs = 1300;
        private const int ImportDialogAutoCloseCanceledMs = 900;
        private const int ImportDialogAutoCloseErrorMs = 1600;

        private readonly NikonSdkDiscovery _nikonSdkDiscovery = new();
        private readonly NikonNefImportService _nikonNefImportService = new();

        private NikonImageSdkLocation? _nikonImageSdkLocation;
        private string? _rawSourceFolder;
        private bool _rawImportBusy;
        private CancellationTokenSource? _rawImportCancellation;

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
                RawSdkStatusText.Foreground = SolidColorBrush.Parse("#FF9A76");
                return;
            }

            RawSdkStatusText.Text = $"Nikon SDK: {_nikonImageSdkLocation.DisplayName} bereit";
            RawSdkStatusText.Foreground = SolidColorBrush.Parse("#99D98C");
        }

        private void UpdateRawImportUiState()
        {
            ImportNefButton.IsEnabled = !_rawImportBusy
                && _nikonImageSdkLocation is not null
                && !string.IsNullOrWhiteSpace(_rawSourceFolder);

            CancelNefImportButton.IsEnabled = _rawImportBusy;

            ImportNefButton.Content = _rawImportBusy ? "NEF-Import laeuft..." : "NEF importieren";
        }

        private async void SelectRawSourceFolder_Click(object sender, RoutedEventArgs e)
        {
            string? folder = await BrowseForFolder("RAW-Ordner auswählen");
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
                await MessageBox.Show(
                    this,
                    "NEF-Import",
                    "Kein passendes Nikon Image SDK gefunden.\nBitte den lokalen SDK-Ordner unter SDKs/Nikon prüfen.");
                UpdateRawImportUiState();
                return;
            }

            if (string.IsNullOrWhiteSpace(_rawSourceFolder))
            {
                await MessageBox.Show(
                    this,
                    "NEF-Import",
                    "Bitte zuerst einen RAW-Ordner mit NEF-Dateien auswählen.");
                return;
            }

            _rawImportBusy = true;
            _rawImportCancellation = new CancellationTokenSource();
            UpdateRawImportUiState();

            ImportProgressDialog progressDialog = new();
            progressDialog.UpdateProgress(0, 1, "Vorbereitung...");
            progressDialog.CancelRequested += CancelNefImportRequested;
            Task progressDialogTask = progressDialog.ShowDialog(this);
            bool showCompletionState = false;
            int completionDelayMs = 0;

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
                progressDialog.UpdateProgress(info.Current, info.Total, info.FileName);
                SetStatus($"NEF-Import {info.Current}/{info.Total}: {info.FileName}");
            });

            try
            {
                StopPlaybackInternal();
                HidePlaybackPreview();
                _playbackIndex = -1;

                NefImportSummary summary = await Task.Run(() =>
                    _nikonNefImportService.ImportFolder(_rawSourceFolder!, settings, progress, _rawImportCancellation.Token));

                if (summary.ImportedCount > 0)
                    ApplyImportedFrames(summary);

                if (summary.WasCanceled)
                {
                    showCompletionState = true;
                    completionDelayMs = ImportDialogAutoCloseCanceledMs;
                    if (summary.ImportedCount > 0)
                    {
                        progressDialog.ShowCompletion("NEF-Import abgebrochen", $"{summary.ImportedCount} Frames bereits importiert.");
                        SetStatus($"NEF-Import abgebrochen: {summary.ImportedCount} Frames wurden bereits importiert.");
                    }
                    else
                    {
                        progressDialog.ShowCompletion("NEF-Import abgebrochen", "Keine Frames importiert.");
                        SetStatus("NEF-Import abgebrochen. Keine Frames importiert.");
                    }
                    return;
                }

                if (summary.HasFailures)
                {
                    showCompletionState = true;
                    completionDelayMs = ImportDialogAutoCloseWarningMs;
                    string details = string.Join("\n", summary.FailedFiles
                        .Take(5)
                        .Select(f => $"- {f.FileName}: {f.Error}"));

                    if (summary.FailedCount > 5)
                        details += $"\n... und {summary.FailedCount - 5} weitere";

                    await MessageBox.Show(
                        this,
                        "NEF-Import mit Warnungen",
                        $"Import abgeschlossen mit Warnungen.\nErfolgreich: {summary.ImportedCount}\nÜbersprungen: {summary.FailedCount}\n\n{details}");

                    progressDialog.ShowCompletion(
                        "NEF-Import mit Warnungen",
                        $"Importiert: {summary.ImportedCount} | Übersprungen: {summary.FailedCount}",
                        isError: true);

                    SetStatus($"NEF-Import mit Warnungen: {summary.ImportedCount} importiert, {summary.FailedCount} übersprungen.");
                }
                else
                {
                    showCompletionState = true;
                    completionDelayMs = ImportDialogAutoCloseSuccessMs;
                    progressDialog.ShowCompletion("NEF-Import abgeschlossen", $"{summary.ImportedCount} Frames importiert.");
                    SetStatus($"NEF-Import fertig: {summary.ImportedCount} Frames nach {summary.MasterFolder}");
                }
            }
            catch (Exception ex)
            {
                showCompletionState = true;
                completionDelayMs = ImportDialogAutoCloseErrorMs;
                progressDialog.ShowCompletion("NEF-Import fehlgeschlagen", ex.Message, isError: true);
                DebugLogger.Instance.LogError("RawImport", $"NEF import failed: {ex.Message}");
                await MessageBox.Show(this, "NEF-Import", ex.Message);
                SetStatus($"NEF-Import fehlgeschlagen: {ex.Message}");
            }
            finally
            {
                if (showCompletionState && completionDelayMs > 0 && progressDialog.IsVisible)
                    await Task.Delay(completionDelayMs);

                progressDialog.CancelRequested -= CancelNefImportRequested;
                if (progressDialog.IsVisible)
                {
                    await Dispatcher.UIThread.InvokeAsync(() => progressDialog.Close());
                }
                await progressDialogTask;

                _rawImportCancellation?.Dispose();
                _rawImportCancellation = null;
                _rawImportBusy = false;
                UpdateRawImportUiState();
            }
        }

        private void CancelNefImportRequested()
        {
            if (!_rawImportBusy)
                return;

            _rawImportCancellation?.Cancel();
            SetStatus("NEF-Import wird abgebrochen...");
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
