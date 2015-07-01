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
    /// Interaction logic for BrisqueWindow.xaml
    /// </summary>
    public partial class BrisqueWindow : Window
    {
        private Brisque _defaultBrisque;
        private Brisque _customBrisque;

        private List<string> _trainingFiles;
        private int _trainingIndex;

        public BrisqueWindow()
        {
            InitializeComponent();

            _trainingFiles = new List<string>();
            _defaultBrisque = new Brisque(ReadResource("ImageQualityClient.default_model"));
            _customBrisque = new Brisque(ReadResource("ImageQualityClient.custom_model"));
        }

        private void ScoreImage_Click(object sender, RoutedEventArgs e)
        {
            ScorePanel.Visibility = Visibility.Visible;
            TrainingPanel.Visibility = Visibility.Collapsed;

            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Image|*.jpg;*.png";
            if (ofd.ShowDialog() == true)
            {
                Image.Source = new BitmapImage(new Uri(ofd.FileName));

                var fileBytes = File.ReadAllBytes(ofd.FileName);

                var features = _defaultBrisque.Features(fileBytes);
                DefaultScoreText.Text = Math.Round(_defaultBrisque.Score(features), 2).ToString();
                CustomScoreText.Text = Math.Round(_customBrisque.Score(features), 2).ToString();
            }
        }

        private void StartTraining_Click(object sender, RoutedEventArgs e)
        {
            ScorePanel.Visibility = Visibility.Collapsed;
            TrainingPanel.Visibility = Visibility.Visible;

            using (var dialog = new System.Windows.Forms.FolderBrowserDialog())
            {
                if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    _trainingFiles = Directory.EnumerateFiles(dialog.SelectedPath, "*.jpg").ToList();
                    _trainingIndex = 0;

                    Image.Source = new BitmapImage(new Uri(_trainingFiles[_trainingIndex]));
                }
            }
        }

        private void GenerateModel_Click(object sender, RoutedEventArgs e)
        {
            var newModel = _defaultBrisque.CreateModel();
            File.WriteAllText("generated_model", newModel);
        }

        private void Skip_Click(object sender, RoutedEventArgs e)
        {
            _trainingIndex++;
            if (_trainingIndex >= _trainingFiles.Count)
            {
                MessageBox.Show("No more files in folder", "Training");
                return;
            }
            Image.Source = new BitmapImage(new Uri(_trainingFiles[_trainingIndex]));
        }

        private void Undo_Click(object sender, RoutedEventArgs e)
        {
            _trainingIndex--;
            if (_trainingIndex < _trainingFiles.Count)
            {
                _trainingIndex = 0;
            }
            Image.Source = new BitmapImage(new Uri(_trainingFiles[_trainingIndex]));
        }

        private void Train_Click(object sender, RoutedEventArgs e)
        {
            var fileBytes = File.ReadAllBytes(_trainingFiles[_trainingIndex]);
            _defaultBrisque.Train(fileBytes, DmosScore());

            _trainingIndex++;
            if (_trainingIndex >= _trainingFiles.Count)
            {
                MessageBox.Show("No more files in folder", "Training");
                return;
            }
            Image.Source = new BitmapImage(new Uri(_trainingFiles[_trainingIndex]));
        }

        private void SaveTraining_Click(object sender, RoutedEventArgs e)
        {
            var training = _defaultBrisque.SaveTraining();
            File.WriteAllLines("training.txt", training);
        }

        private void LoadTraining_Click(object sender, RoutedEventArgs e)
        {
            if (File.Exists("training.txt"))
            {
                var training = File.ReadAllLines("training.txt");
                _defaultBrisque.ResumeTraining(training.ToList());
            }
        }

        private float DmosScore()
        {
            if (rbAwful.IsChecked == true) return 100;
            if (rbPoor.IsChecked == true) return 75;
            if (rbOkay.IsChecked == true) return 50;
            if (rbGood.IsChecked == true) return 25;
            if (rbPerfect.IsChecked == true) return 0;
            return 50;
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