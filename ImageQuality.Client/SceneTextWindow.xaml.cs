using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;
using ImageQuality;
using Microsoft.Win32;

namespace ImageQualityClient
{
    /// <summary>
    /// Interaction logic for SceneTextWindow.xaml
    /// </summary>
    public partial class SceneTextWindow : Window
    {
        private SceneText _sceneText;

        public SceneTextWindow()
        {
            InitializeComponent();
            _sceneText = new SceneText();
        }

        private void DetectText_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Image|*.jpg;*.png";
            if (ofd.ShowDialog() == true)
            {
                Image.Source = new BitmapImage(new Uri(ofd.FileName));

                var fileBytes = File.ReadAllBytes(ofd.FileName);

                ResultText.Text = _sceneText.DetectRegions(fileBytes).Trim();
            }
        }
    }
}