using Avalonia.Controls;
using Avalonia.Controls.ApplicationLifetimes;
using System.Collections.ObjectModel;
using System.IO;

namespace OpenStopMotionStudio.GUI
{
    public partial class ProjectWindow : Window
    {
        private readonly IClassicDesktopStyleApplicationLifetime? _desktop;
        public ObservableCollection<ProjectViewModel> Projects { get; } = new();
        public ProjectViewModel? SelectedProject { get; set; }

        // This constructor is used by the designer.
        public ProjectWindow()
        {
            InitializeComponent();
            DataContext = this;
            // In designer mode, _desktop is null, so don't add button handlers.
        }

        public ProjectWindow(IClassicDesktopStyleApplicationLifetime desktop)
        {
            InitializeComponent();
            DataContext = this;
            _desktop = desktop;

            ScanForProjects();

            NewProjectButton.Click += async (s, e) =>
            {
                var dialog = new NewProjectDialog();
                var result = await dialog.ShowDialog(this);

                if (result != null && !string.IsNullOrWhiteSpace(result.Name) && !string.IsNullOrWhiteSpace(result.Location))
                {
                    // In a real app, you would create the project directory structure here.
                    var projectPath = Path.Combine(result.Location, result.Name);
                    Directory.CreateDirectory(projectPath); // Create the project folder
                    
                    OpenMainWindow(projectPath);
                }
            };

            OpenProjectButton.Click += (s, e) =>
            {
                if (SelectedProject?.FullPath != null)
                {
                    OpenMainWindow(SelectedProject.FullPath);
                }
            };
        }

        private void ScanForProjects()
        {
            Projects.Clear();
            var basePath = Path.Combine(
                System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), 
                "OpenStopMotionStudio");

            if (Directory.Exists(basePath))
            {
                foreach (var dir in Directory.EnumerateDirectories(basePath))
                {
                    Projects.Add(new ProjectViewModel
                    {
                        Name = Path.GetFileName(dir),
                        FullPath = dir
                    });
                }
            }
        }

        private void OpenMainWindow(string projectPath)
        {
            if (_desktop == null) return;
            var mainWindow = new MainWindow(projectPath);
            _desktop.MainWindow = mainWindow;
            mainWindow.Show();
            Close(); // Close the project browser
        }
    }
}
