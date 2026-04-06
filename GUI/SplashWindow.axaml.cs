using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Controls.ApplicationLifetimes;
using OpenStopMotionStudio.Core;
using System;

namespace OpenStopMotionStudio.GUI
{
    public partial class SplashWindow : Window
    {
        private readonly IClassicDesktopStyleApplicationLifetime _desktop;
        private readonly SplashViewModel _viewModel;

        public SplashWindow() : this(null!) { }

        public SplashWindow(IClassicDesktopStyleApplicationLifetime desktop)
        {
            InitializeComponent();
            _desktop = desktop;
            _viewModel = new SplashViewModel();
            DataContext = _viewModel;

            Loaded += OnLoaded;
        }

        private async void OnLoaded(object? sender, RoutedEventArgs e)
        {
            // The event handler must be async void to run on the UI thread
            var initializationService = new InitializationService();
            bool success = await initializationService.RunAsync(status =>
            {
                // This action is the callback to update the UI
                _viewModel.Status = status;
            }, this);

            if (success)
            {
                var projectWindow = new ProjectWindow(_desktop);
                _desktop.MainWindow = projectWindow;
                projectWindow.Show();
            }
            // On failure, the service will have shown an error. Just close.
            
            Close();
        }
    }
}
