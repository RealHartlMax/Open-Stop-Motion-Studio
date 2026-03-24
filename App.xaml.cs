using System.Windows;

namespace OpenStopMotionStudio
{
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            // Globaler Exception-Handler: verhindert stille Abstürze
            DispatcherUnhandledException += (sender, args) =>
            {
                MessageBox.Show(
                    $"Unerwarteter Fehler:\n{args.Exception.Message}",
                    "Open Stop Motion Studio – Fehler",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error);
                args.Handled = true;
            };
        }
    }
}
