using Avalonia.Controls;
using Avalonia.Interactivity;
using OpenStopMotionStudio.Core;
using System;
using System.Diagnostics;
using System.Resources;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.GUI
{
    public partial class UpdateAvailableWindow : Window
    {
        private static readonly ResourceManager ResourceManager = new("OpenStopMotionStudio.Localization.Strings", typeof(UpdateAvailableWindow).Assembly);
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
            dialog.TitleText.Text = ResourceManager.GetString("UpdateAvailable_Title") ?? "A new version is available";
            dialog.VersionText.Text = string.Format(ResourceManager.GetString("UpdateAvailable_VersionFormat") ?? "Current: {0} | New: {1}", update.CurrentVersion, update.LatestVersion);
            dialog.NotesText.Text = string.IsNullOrWhiteSpace(update.ReleaseNotes)
                ? ResourceManager.GetString("UpdateAvailable_NoReleaseNotes") ?? "No release notes provided."
                : update.ReleaseNotes;
            dialog.LaterButton.Content = ResourceManager.GetString("UpdateAvailable_Later") ?? "Later";
            dialog.UpdateButton.Content = ResourceManager.GetString("UpdateAvailable_Update") ?? "Update";
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
