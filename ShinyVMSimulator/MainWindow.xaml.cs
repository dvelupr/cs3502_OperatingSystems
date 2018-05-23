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
using System.Windows.Shapes;
using ShinyVMSimulator.ViewModels;

namespace ShinyVMSimulator
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void HomeButton_Click(object sender, RoutedEventArgs e)
        {
            DataContext = new HomeViewModel();
        }

        private void SystemInfoButton_Click(object sender, RoutedEventArgs e)
        {
            DataContext = new SystemInfoViewModel();
        }

        private void ProcessorInfoButton_Click(object sender, RoutedEventArgs e)
        {
            DataContext = new ProcessorInfoViewModel();
        }

        private void MemoryInfoButton_Click(object sender, RoutedEventArgs e)
        {
            DataContext = new MemoryInfoViewModel();
        }

        private void SchedulingInfoButton_Click(object sender, RoutedEventArgs e)
        {
            DataContext = new SchedulingInfoViewModel();
        }

        private void TerminalButton_Click(object sender, RoutedEventArgs e)
        {
            DataContext = new TerminalViewModel();
        }
    }
}
