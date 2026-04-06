using Avalonia.Controls;
using Avalonia.Platform.Storage;
using System.IO;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.GUI
{
    public class NewProjectDialogResult
    {
        public string? Name { get; set; }
        public string? Location { get; set; }
    }

    public partial class NewProjectDialog : Window
    {
        public NewProjectDialog()
        {
            InitializeComponent();
            
            // Set a default location
            LocationTextBox.Text = Path.Combine(
                System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), 
                "OpenStopMotionStudio");

            CancelButton.Click += (sender, e) => Close(null);
            CreateButton.Click += (sender, e) => 
            {
                var result = new NewProjectDialogResult
                {
                    Name = NameTextBox.Text,
                    Location = LocationTextBox.Text
                };
                Close(result);
            };
            ChangeLocationButton.Click += async (sender, e) =>
            {
                var folder = await StorageProvider.OpenFolderPickerAsync(new FolderPickerOpenOptions 
                { 
                    Title = "Speicherort auswählen"
                });

                if (folder.Count > 0)
                {
                    LocationTextBox.Text = folder[0].Path.LocalPath;
                }
            };
        }

        public new Task<NewProjectDialogResult?> ShowDialog(Window owner)
        {
            return ShowDialog<NewProjectDialogResult?>(owner);
        }
    }
}
