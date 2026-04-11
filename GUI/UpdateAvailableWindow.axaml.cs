using Avalonia.Controls;
using Avalonia.Interactivity;
using OpenStopMotionStudio.Core;
using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.GUI
{
    public partial class UpdateAvailableWindow : Window
    {
        private string _downloadUrl = string.Empty;

        public UpdateAvailableWindow()
        {
            InitializeComponent();
        }

        public static async Task<bool> ShowIfAvailableAsync(Window owner, UpdateCheckResult update)
        {
            if (!update.IsUpdateAvailable)
                return false;

            var dialog = new UpdateAvailableWindow();
            dialog.VersionText.Text = $"Current: {update.CurrentVersion} | New: {update.LatestVersion}";
            dialog.NotesText.Text = string.IsNullOrWhiteSpace(update.ReleaseNotes)
                ? "No release notes provided."
                : update.ReleaseNotes;
            dialog._downloadUrl = update.DownloadUrl;
            dialog.UpdateButton.IsEnabled = !string.IsNullOrWhiteSpace(update.DownloadUrl);

            bool? result = await dialog.ShowDialog<bool?>(owner);
            return result == true;
        }

        private void LaterButton_Click(object? sender, RoutedEventArgs e)
        {
            Close(false);
        }

        private void UpdateButton_Click(object? sender, RoutedEventArgs e)
        {
            if (!Uri.TryCreate(_downloadUrl, UriKind.Absolute, out Uri? uri))
            {
                Close(false);
                return;
            }

            try
            {
                Process.Start(new ProcessStartInfo
                {
                    FileName = uri.ToString(),
                    UseShellExecute = true
                });
                Close(true);
            }
            catch
            {
                Close(false);
            }
        }
    }
}
