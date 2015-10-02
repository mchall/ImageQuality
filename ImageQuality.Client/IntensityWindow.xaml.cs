using System;
using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;
using ImageQuality;
using Microsoft.Win32;

namespace ImageQualityClient
{
    /// <summary>
    /// Interaction logic for IntensityWindow.xaml
    /// </summary>
    public partial class IntensityWindow : Window
    {
        private Intensity _intensity;

        public IntensityWindow()
        {
            InitializeComponent();
            _intensity = new Intensity();
        }

        private void DetectIntensity_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Image|*.jpg;*.png";
            if (ofd.ShowDialog() == true)
            {
                var fileBytes = File.ReadAllBytes(ofd.FileName);

                var sw = Stopwatch.StartNew();

                ResultText.Text = String.Format("Intensity: {0}", Math.Round(_intensity.IntensityMeasure(fileBytes), 2));

                sw.Stop();
                TimeText.Text = String.Format("{0}ms", sw.ElapsedMilliseconds);

                MemoryStream ms = new MemoryStream(fileBytes);
                var imageSource = new BitmapImage();
                imageSource.BeginInit();
                imageSource.StreamSource = ms;
                imageSource.EndInit();

                Image.Source = imageSource;
            }
        }
    }
}