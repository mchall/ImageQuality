using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

namespace ImageQualityClient
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

        private void Brisque_Click(object sender, RoutedEventArgs e)
        {
            BrisqueWindow window = new BrisqueWindow();
            window.ShowDialog();
        }

        private void SceneText_Click(object sender, RoutedEventArgs e)
        {
            SceneTextWindow window = new SceneTextWindow();
            window.ShowDialog();
        }
    }
}