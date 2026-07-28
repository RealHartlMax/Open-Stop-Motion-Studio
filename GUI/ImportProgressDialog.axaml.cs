using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Media;
using System;
using System.Resources;

namespace OpenStopMotionStudio.GUI
{
    public partial class ImportProgressDialog : Window
    {
        private readonly ResourceManager _resourceManager = new("OpenStopMotionStudio.Localization.Strings", typeof(ImportProgressDialog).Assembly);

        public event Action? CancelRequested;

        public ImportProgressDialog()
        {
            InitializeComponent();
        }

        public void UpdateProgress(int current, int total, string fileName)
        {
            total = Math.Max(total, 1);
            int safeCurrent = Math.Clamp(current, 0, total);
            double percent = (safeCurrent * 100.0) / total;

            ProgressTextBlock.Text = $"{safeCurrent} / {total}";
            ImportProgressBar.Value = percent;
            CurrentFileTextBlock.Text = string.IsNullOrWhiteSpace(fileName)
                ? _resourceManager.GetString("ImportProgressDialog_Processing") ?? "Processing..."
                : fileName;
        }

        public void ShowCompletion(string title, string details, bool isError = false)
        {
            TitleTextBlock.Text = title;
            CurrentFileTextBlock.Text = details;
            CancelButton.IsEnabled = false;
            CancelButton.Content = _resourceManager.GetString("ImportProgressDialog_Close") ?? "Close";

            if (isError)
                TitleTextBlock.Foreground = SolidColorBrush.Parse("#FF9A76");
            else
                TitleTextBlock.Foreground = SolidColorBrush.Parse("#99D98C");
        }

        private void CancelButton_Click(object? sender, RoutedEventArgs e)
        {
            CancelButton.IsEnabled = false;
            CancelButton.Content = _resourceManager.GetString("ImportProgressDialog_Stopping") ?? "Stopping...";
            TitleTextBlock.Text = _resourceManager.GetString("ImportProgressDialog_Canceling") ?? "Import is being canceled...";
            CancelRequested?.Invoke();
        }
    }
}
