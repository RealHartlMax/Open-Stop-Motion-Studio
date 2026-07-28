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
                RawSdkStatusText.Text = _resourceManager.GetString("RawImport_NikonSdkNotFound") ?? "Nikon SDK: no local NkImgSDK found. Please check SDKs/Nikon.";
                RawSdkStatusText.Foreground = SolidColorBrush.Parse("#FF9A76");
                return;
            }

            RawSdkStatusText.Text = string.Format(_resourceManager.GetString("RawImport_NikonSdkReadyFormat") ?? "Nikon SDK: {0} ready", _nikonImageSdkLocation.DisplayName);
            RawSdkStatusText.Foreground = SolidColorBrush.Parse("#99D98C");
        }

        private void UpdateRawImportUiState()
        {
            ImportNefButton.IsEnabled = !_rawImportBusy
                && _nikonImageSdkLocation is not null
                && !string.IsNullOrWhiteSpace(_rawSourceFolder);

            CancelNefImportButton.IsEnabled = _rawImportBusy;

            ImportNefButton.Content = _rawImportBusy
                ? _resourceManager.GetString("RawImport_ImportButton_Busy") ?? "NEF import in progress..."
                : _resourceManager.GetString("RawImport_ImportButton_Idle") ?? "Import NEF";
        }

        private async void SelectRawSourceFolder_Click(object sender, RoutedEventArgs e)
        {
            string? folder = await BrowseForFolder(_resourceManager.GetString("RawImport_SelectFolderTitle") ?? "Select RAW folder");
            if (string.IsNullOrWhiteSpace(folder))
                return;

            _rawSourceFolder = folder;
            RawSourceFolderText.Text = folder;
            UpdateRawImportUiState();
            SetStatus(string.Format(_resourceManager.GetString("RawImport_SourceFolderStatusFormat") ?? "RAW source: {0}", folder));
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
                    _resourceManager.GetString("RawImport_Title") ?? "NEF import",
                    _resourceManager.GetString("RawImport_NoSdkFoundMessage") ?? "No suitable Nikon Image SDK found.\nPlease check the local SDK folder under SDKs/Nikon.");
                UpdateRawImportUiState();
                return;
            }

            if (string.IsNullOrWhiteSpace(_rawSourceFolder))
            {
                await MessageBox.Show(
                    this,
                    _resourceManager.GetString("RawImport_Title") ?? "NEF import",
                    _resourceManager.GetString("RawImport_NoSourceFolderMessage") ?? "Please select a RAW folder containing NEF files first.");
                return;
            }

            _rawImportBusy = true;
            _rawImportCancellation = new CancellationTokenSource();
            UpdateRawImportUiState();

            ImportProgressDialog progressDialog = new();
            progressDialog.UpdateProgress(0, 1, _resourceManager.GetString("RawImport_Preparing") ?? "Preparing...");
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
                SetStatus(string.Format(_resourceManager.GetString("RawImport_ProgressStatusFormat") ?? "NEF import {0}/{1}: {2}", info.Current, info.Total, info.FileName));
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
                        progressDialog.ShowCompletion(_resourceManager.GetString("RawImport_CanceledTitle") ?? "NEF import canceled", string.Format(_resourceManager.GetString("RawImport_CanceledWithFramesMessage") ?? "{0} frames already imported.", summary.ImportedCount));
                        SetStatus(string.Format(_resourceManager.GetString("RawImport_CanceledStatusWithFrames") ?? "NEF import canceled: {0} frames were already imported.", summary.ImportedCount));
                    }
                    else
                    {
                        progressDialog.ShowCompletion(_resourceManager.GetString("RawImport_CanceledTitle") ?? "NEF import canceled", _resourceManager.GetString("RawImport_CanceledNoFramesMessage") ?? "No frames imported.");
                        SetStatus(_resourceManager.GetString("RawImport_CanceledNoFramesStatus") ?? "NEF import canceled. No frames imported.");
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
                        _resourceManager.GetString("RawImport_WarningsTitle") ?? "NEF import with warnings",
                        string.Format(_resourceManager.GetString("RawImport_WarningsMessage") ?? "Import completed with warnings.\nSuccessful: {0}\nSkipped: {1}\n\n{2}", summary.ImportedCount, summary.FailedCount, details));

                    progressDialog.ShowCompletion(
                        _resourceManager.GetString("RawImport_WarningsTitle") ?? "NEF import with warnings",
                        string.Format(_resourceManager.GetString("RawImport_WarningsSummary") ?? "Imported: {0} | Skipped: {1}", summary.ImportedCount, summary.FailedCount),
                        isError: true);

                    SetStatus(string.Format(_resourceManager.GetString("RawImport_WarningsStatus") ?? "NEF import with warnings: {0} imported, {1} skipped.", summary.ImportedCount, summary.FailedCount));
                }
                else
                {
                    showCompletionState = true;
                    completionDelayMs = ImportDialogAutoCloseSuccessMs;
                    progressDialog.ShowCompletion(_resourceManager.GetString("RawImport_CompletedTitle") ?? "NEF import completed", string.Format(_resourceManager.GetString("RawImport_CompletedSummary") ?? "{0} frames imported.", summary.ImportedCount));
                    SetStatus(string.Format(_resourceManager.GetString("RawImport_CompletedStatus") ?? "NEF import finished: {0} frames to {1}", summary.ImportedCount, summary.MasterFolder));
                }
            }
            catch (Exception ex)
            {
                showCompletionState = true;
                completionDelayMs = ImportDialogAutoCloseErrorMs;
                progressDialog.ShowCompletion(_resourceManager.GetString("RawImport_FailedTitle") ?? "NEF import failed", ex.Message, isError: true);
                DebugLogger.Instance.LogError("RawImport", $"NEF import failed: {ex.Message}");
                await MessageBox.Show(this, _resourceManager.GetString("RawImport_Title") ?? "NEF import", ex.Message);
                SetStatus(string.Format(_resourceManager.GetString("RawImport_FailedStatus") ?? "NEF import failed: {0}", ex.Message));
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
            SetStatus(_resourceManager.GetString("RawImport_CancelingStatus") ?? "NEF import is being canceled...");
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
