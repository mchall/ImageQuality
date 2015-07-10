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
    /// Interaction logic for SimpleWatermarkWindow.xaml
    /// </summary>
    public partial class SimpleWatermarkWindow : Window
    {
        private ImageTextExtractor _textExtractor;

        public SimpleWatermarkWindow()
        {
            InitializeComponent();
            _textExtractor = new ImageTextExtractor();
        }

        private void DetectText_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Image|*.jpg;*.png";
            if (ofd.ShowDialog() == true)
            {
                var fileBytes = File.ReadAllBytes(ofd.FileName);

                var sw = Stopwatch.StartNew();

                ResultText.Text = _textExtractor.WatermarkDetect(fileBytes).Trim();

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