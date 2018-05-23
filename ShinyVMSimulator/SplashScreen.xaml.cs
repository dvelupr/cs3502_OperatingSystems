using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace ShinyVMSimulator
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class SplashScreen : Window
    {
        private DispatcherTimer mSplashScreenTimer;

        public SplashScreen()
        {
            InitializeComponent();

            mSplashScreenTimer = new DispatcherTimer();
            mSplashScreenTimer.Tick += new EventHandler(splashScreen_Tick);
            mSplashScreenTimer.Interval = new TimeSpan(0, 0, 0, 0, 2000);
            mSplashScreenTimer.Start();
        }

        private void splashScreen_Tick(object sender, EventArgs e)
        {
            MainWindow mainWindow = new MainWindow();
            mainWindow.Show();

            mSplashScreenTimer.Stop();
            this.Close();
        }
    }
}
