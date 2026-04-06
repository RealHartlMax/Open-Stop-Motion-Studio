using Avalonia.Controls;
using Avalonia.Interactivity;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.GUI
{
    public partial class MessageBox : Window
    {
        public MessageBox()
        {
            InitializeComponent();
        }

        public static Task Show(Window parent, string title, string message)
        {
            var msgbox = new MessageBox
            {
                Title = title
            };
            msgbox.TitleTextBlock.Text = title;
            msgbox.MessageTextBlock.Text = message;
            
            return msgbox.ShowDialog(parent);
        }

        private void OkButton_Click(object? sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}
