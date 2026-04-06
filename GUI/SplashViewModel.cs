using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace OpenStopMotionStudio.GUI
{
    public class SplashViewModel : INotifyPropertyChanged
    {
        private string _status = "Initializing...";

        public string Status
        {
            get => _status;
            set
            {
                if (_status != value)
                {
                    _status = value;
                    OnPropertyChanged();
                }
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
