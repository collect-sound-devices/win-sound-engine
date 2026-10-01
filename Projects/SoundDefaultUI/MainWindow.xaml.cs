using System.ComponentModel;
using System.Windows;
using System.Windows.Interop;

namespace SoundDefaultUI
{
    public partial class MainWindow
    {
        public MainWindow(MainViewModel mainWindowViewModel)
        {
            InitializeComponent();
            DataContext = mainWindowViewModel;
        }

        protected override void OnSourceInitialized(EventArgs e)
        {
            base.OnSourceInitialized(e);
            TaskbarIdentity.Set(new WindowInteropHelper(this).Handle, ((MainViewModel)DataContext).WindowTitle);
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            base.OnClosing(e);
            var handle = new WindowInteropHelper(this).Handle;
            if (!e.Cancel && handle != IntPtr.Zero)
            {
                TaskbarIdentity.Clear(handle);
            }
        }

        private void CloseWindow(object sender, RoutedEventArgs e)
            => SystemCommands.CloseWindow(this);
    }
}
