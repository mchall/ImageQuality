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
    /// Interaction logic for HistogramWindow.xaml
    /// </summary>
    public partial class HistogramWindow : Window
    {
        private HistogramHelper _histogram;

        public HistogramWindow()
        {
            InitializeComponent();
            _histogram = new HistogramHelper();
        }

        private void CompareHistogram_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Image|*.jpg;*.png";
            if (ofd.ShowDialog() == true)
            {
                var left = File.ReadAllBytes(ofd.FileName);

                if (ofd.ShowDialog() == true)
                {
                    var right = File.ReadAllBytes(ofd.FileName);

                    var sw = Stopwatch.StartNew();

                    ResultText.Text = String.Format("Score: {0}", Math.Round(_histogram.CompareHistograms(left, right), 2));

                    sw.Stop();
                    TimeText.Text = String.Format("{0}ms", sw.ElapsedMilliseconds);

                    MemoryStream ms = new MemoryStream(left);
                    var imageSource = new BitmapImage();
                    imageSource.BeginInit();
                    imageSource.StreamSource = ms;
                    imageSource.EndInit();

                    ImageLeft.Source = imageSource;

                    MemoryStream ms2 = new MemoryStream(right);
                    var imageSource2 = new BitmapImage();
                    imageSource2.BeginInit();
                    imageSource2.StreamSource = ms2;
                    imageSource2.EndInit();
                    ImageRight.Source = imageSource2;
                }
            }
        }
    }
}