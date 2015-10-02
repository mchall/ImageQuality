using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;
using ImageQuality;

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

        private void SimleWatermark_Click(object sender, RoutedEventArgs e)
        {
            SimpleWatermarkWindow window = new SimpleWatermarkWindow();
            window.ShowDialog();
        }

        private void Blur_Click(object sender, RoutedEventArgs e)
        {
            BlurWindow window = new BlurWindow();
            window.ShowDialog();
        }

        private void Intensity_Click(object sender, RoutedEventArgs e)
        {
            IntensityWindow window = new IntensityWindow();
            window.ShowDialog();
        }

        private void Bulk_Click(object sender, RoutedEventArgs e)
        {
            var progress = new ProgressWindow();
            progress.Owner = this;
            progress.Show();

            BackgroundWorker worker = new BackgroundWorker();
            worker.WorkerReportsProgress = true;
            worker.DoWork += (s, ev) =>
                {
                    ImageTextExtractor extractor = new ImageTextExtractor();
                    var folder = @"C:\Users\michaelha\Desktop\SuburbDownload";
                    var files = Directory.EnumerateFiles(folder).ToList();

                    int count = 0;
                    foreach (var file in files)
                    {
                        var b = File.ReadAllBytes(file);
                        byte[] debug;
                        var text = extractor.NaturalSceneDetect(b, out debug);
                        if (!String.IsNullOrEmpty(text))
                        {
                            File.WriteAllBytes(Path.Combine(@"C:\Users\michaelha\Desktop\OUTPUT", Path.GetFileName(file)), debug);
                        }
                        count++;

                        var p = ((double)count / (double)files.Count) * 100;
                        worker.ReportProgress((int)p, p);
                    }
                };
            worker.RunWorkerCompleted += (s, ev) =>
                {
                    progress.Close();
                };
            worker.ProgressChanged += (s, ev) =>
                {
                    var p = (double)ev.UserState;
                    progress.UpdateProgress(p);
                };
            worker.RunWorkerAsync();            
        }

        private string ReadResource(string resourceName)
        {
            var assembly = Assembly.GetExecutingAssembly();
            using (Stream stream = assembly.GetManifestResourceStream(resourceName))
            using (StreamReader reader = new StreamReader(stream))
            {
                return reader.ReadToEnd();
            }
        }
    }
}