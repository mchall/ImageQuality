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
    /// Interaction logic for ColorWindow.xaml
    /// </summary>
    public partial class ColorWindow : Window
    {
        private ColorHelper _color;

        public ColorWindow()
        {
            InitializeComponent();
            _color = new ColorHelper();
        }

        private void ColorInfo_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Image|*.jpg;*.png";
            if (ofd.ShowDialog() == true)
            {
                var fileBytes = File.ReadAllBytes(ofd.FileName);

                var sw = Stopwatch.StartNew();

                var info = _color.ColorInfo(fileBytes);
                ResultText.Text = String.Format("Red: {0}\nGreen: {1}\nBlue: {2}\nYellow: {3}", Math.Round(info.Red, 2), Math.Round(info.Green, 2), Math.Round(info.Blue, 2), Math.Round(info.Yellow, 2));

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