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
        private string _left;
        private string _right;
        private HistogramHelper _histogram;

        public HistogramWindow()
        {
            InitializeComponent();
            _histogram = new HistogramHelper();
        }

        private void LoadLeft_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Image|*.jpg;*.png";
            if (ofd.ShowDialog() == true)
            {
                _left = ofd.FileName;
                if (_right != null)
                {
                    DoCompare();
                }
            }
        }

        private void LoadRight_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Image|*.jpg;*.png";
            if (ofd.ShowDialog() == true)
            {
                _right = ofd.FileName;
                if (_left != null)
                {
                    DoCompare();
                }
            }
        }

        private void DoCompare()
        {
            var left = File.ReadAllBytes(_left);
            var right = File.ReadAllBytes(_right);

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