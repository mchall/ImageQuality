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
    /// Interaction logic for BlurWindow.xaml
    /// </summary>
    public partial class BlurWindow : Window
    {
        private Blur _blur;

        public BlurWindow()
        {
            InitializeComponent();
            _blur = new Blur();
        }

        private void DetectBlur_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Image|*.jpg;*.png";
            if (ofd.ShowDialog() == true)
            {
                var fileBytes = File.ReadAllBytes(ofd.FileName);

                var sw = Stopwatch.StartNew();

                ResultText.Text = String.Format("Blur: {0}", Math.Round(_blur.BlurMeasure(fileBytes), 2));

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