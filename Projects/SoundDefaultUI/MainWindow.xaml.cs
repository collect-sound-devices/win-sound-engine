using System.Windows;

namespace SoundDefaultUI
{
    public partial class MainWindow
    {
        public MainWindow(MainViewModel mainWindowViewModel)
        {
            InitializeComponent();
            DataContext = mainWindowViewModel;
        }

        private void CloseWindow(object sender, RoutedEventArgs e)
            => SystemCommands.CloseWindow(this);
    }
}
