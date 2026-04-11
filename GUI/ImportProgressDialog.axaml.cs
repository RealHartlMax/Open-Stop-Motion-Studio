using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Media;
using System;

namespace OpenStopMotionStudio.GUI
{
    public partial class ImportProgressDialog : Window
    {
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
            CurrentFileTextBlock.Text = string.IsNullOrWhiteSpace(fileName) ? "Verarbeitung..." : fileName;
        }

        public void ShowCompletion(string title, string details, bool isError = false)
        {
            TitleTextBlock.Text = title;
            CurrentFileTextBlock.Text = details;
            CancelButton.IsEnabled = false;
            CancelButton.Content = "Schließen";

            if (isError)
                TitleTextBlock.Foreground = SolidColorBrush.Parse("#FF9A76");
            else
                TitleTextBlock.Foreground = SolidColorBrush.Parse("#99D98C");
        }

        private void CancelButton_Click(object? sender, RoutedEventArgs e)
        {
            CancelButton.IsEnabled = false;
            CancelButton.Content = "Stoppt...";
            TitleTextBlock.Text = "Import wird abgebrochen...";
            CancelRequested?.Invoke();
        }
    }
}
