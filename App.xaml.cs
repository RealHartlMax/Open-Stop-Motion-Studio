using System;
using System.IO;
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
                Exception rootException = UnwrapException(args.Exception);
                WriteUnhandledExceptionLog(args.Exception);

                MessageBox.Show(
                    $"Unerwarteter Fehler:\n{rootException.GetType().Name}: {rootException.Message}",
                    "Open Stop Motion Studio – Fehler",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error);
                args.Handled = true;
            };
        }

        private static Exception UnwrapException(Exception exception)
        {
            while (exception.InnerException != null)
                exception = exception.InnerException;

            return exception;
        }

        private static void WriteUnhandledExceptionLog(Exception exception)
        {
            try
            {
                string logPath = Path.Combine(AppContext.BaseDirectory, "open-stop-motion-studio-error.log");
                string logEntry =
                    $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] Unhandled exception{Environment.NewLine}{exception}{Environment.NewLine}{Environment.NewLine}";

                File.AppendAllText(logPath, logEntry);
            }
            catch
            {
                // Logging must never crash the global exception handler.
            }
        }
    }
}
