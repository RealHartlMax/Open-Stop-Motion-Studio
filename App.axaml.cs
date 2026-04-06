using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;
using Avalonia.Platform.Storage;
using OpenStopMotionStudio.GUI;
using Avalonia.Threading;
using System;
using System.IO;

namespace OpenStopMotionStudio
{
    public partial class App : Application
    {
        public override void Initialize()
        {
            AvaloniaXamlLoader.Load(this);
        }

        public override void OnFrameworkInitializationCompleted()
        {
            if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
            {
                var splashWindow = new SplashWindow(desktop);
                // We don't set MainWindow here because the splash screen is temporary.
                // The MainWindow will be set by the SplashWindow itself once init is complete.
                splashWindow.Show();
            }

            base.OnFrameworkInitializationCompleted();
        }
    }
}
