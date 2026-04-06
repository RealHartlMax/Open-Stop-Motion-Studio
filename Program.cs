using Avalonia;
using System;
using System.IO;
using System.Threading.Tasks;

namespace OpenStopMotionStudio;

class Program
{
    // Initialization code. Don't use any Avalonia, third-party APIs or any
    // SynchronizationContext-reliant code before AppMain is called: things aren't initialized
    // yet and stuff might break.
    [STAThread]
    public static void Main(string[] args)
    {
        AppDomain.CurrentDomain.UnhandledException += (sender, e) =>
            HandleException(e.ExceptionObject as Exception, "AppDomain.CurrentDomain.UnhandledException");

        try
        {
            BuildAvaloniaApp()
                .StartWithClassicDesktopLifetime(args);
        }
        catch (Exception e)
        {
            HandleException(e, "Avalonia Startup");
        }
    }

    // Avalonia configuration, don't remove; also used by visual designer.
    public static AppBuilder BuildAvaloniaApp()
        => AppBuilder.Configure<App>()
            .UsePlatformDetect()
            .LogToTrace()
            .AfterSetup(_ =>
            {
                TaskScheduler.UnobservedTaskException += (sender, e) =>
                    HandleException(e.Exception, "TaskScheduler.UnobservedTaskException");
            });

    public static void HandleException(Exception? ex, string context)
    {
        if (ex == null) return;

        var exception = UnwrapException(ex);
        WriteUnhandledExceptionLog(exception, context);
    }

    private static Exception UnwrapException(Exception exception)
    {
        while (exception.InnerException != null)
            exception = exception.InnerException;

        return exception;
    }

    private static void WriteUnhandledExceptionLog(Exception exception, string context)
    {
        try
        {
            string logFolder = Path.Combine(AppContext.BaseDirectory, "Logs");
            if (!Directory.Exists(logFolder))
                Directory.CreateDirectory(logFolder);

            string logPath = Path.Combine(logFolder, "open-stop-motion-studio-error.log");
            string logEntry =
                $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] Unhandled exception in {context}{Environment.NewLine}{exception}{Environment.NewLine}{Environment.NewLine}";

            File.AppendAllText(logPath, logEntry);
        }
        catch
        {
            // Logging must never crash the global exception handler.
        }
    }
}
