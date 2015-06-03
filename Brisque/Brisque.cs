using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using SVM;

namespace Brisque
{
    public class Brisque
    {
        private BrisqueFeatureExtractor _extractor;
        private Model _model;
        private List<string> _trainingData;

        private RangeTransform _transform;
        private RangeTransform Transform
        {
            get
            {
                if (_transform == null)
                {
                    double[] minValues = new double[36]
                    {
                        0.251, 0.03307933, 0.203, -0.2937322, 0.001604942, 0.008229790000000001, 0.202, -0.2025705, 0.0007843808000000001, 0.01004946, 0.209, -0.2742442, 0.00212215, 0.006154552, 0.206, -0.2780814, 0.002075887, 0.008071138, 0.302, 0.05337101, 0.244, -0.3052172, 0.01907686, 0.01302931, 0.223, -0.169075, 0.007364005, 0.01690972, 0.24, -0.3070602, 0.02127823, 0.01111699, 0.244, -0.3098141, 0.02249892, 0.01092846
                    };
                    double[] maxValues = new double[36]
                    {
                        4.285, 0.6114288, 1.087, 0.1029576, 0.5260857, 0.331496, 1.08, 0.2669844, 0.4949559, 0.3789517, 1.129, 0.07478476000000001, 0.4927376, 0.3362183, 1.133, 0.07490148000000001, 0.487952, 0.2883914, 3.635, 0.5454887000000001, 1.008, 0.1270696, 0.5678025, 0.3268676, 0.978, 0.1976776, 0.4143375, 0.3387994, 1.183, 0.02245884, 0.5251434, 0.249476, 1.167, 0.05944538, 0.5307955, 0.2452182
                    };
                    _transform = new RangeTransform(minValues, maxValues, -1, 1);
                }
                return _transform;
            }
        }

        public Brisque(string model)
        {
            _extractor = new BrisqueFeatureExtractor();
            _trainingData = new List<string>();
            _model = Model.Read(ToStream(model));
        }

        public IList<double> Features(byte[] fileBytes)
        {
            var extractor = new BrisqueFeatureExtractor();
            return extractor.BrisqueFeatures(fileBytes);
        }

        public double Score(IList<double> features)
        {
            var problem = new Problem(1, new double[1] { 1 }, ToNode(features), features.Count); //36?
            var scaled = Transform.Scale(problem);

            return _model.Predict(scaled.X[0]);
        }

        public double Score(byte[] fileBytes)
        {
            var features = _extractor.BrisqueFeatures(fileBytes);
            return Score(features);
        }

        public void Train(byte[] fileBytes, float dmos)
        {
            var features = Features(fileBytes);
            var t = GetTrainingData(features, dmos);
            _trainingData.Add(t);
        }

        public List<string> SaveTraining()
        {
            return _trainingData;
        }

        public void ResumeTraining(List<string> trainingData)
        {
            _trainingData = trainingData;
        }

        public string CreateModel(List<string> trainingData)
        {
            _trainingData = trainingData;
            return CreateModel();
        }

        public string CreateModel()
        {
            StringBuilder sb = new StringBuilder();
            _trainingData.ForEach(t => sb.Append(t));
            var stream = ToStream(sb.ToString());

            var problem = Problem.Read(stream);
            var scaled = Transform.Scale(problem);

            Parameter p = new Parameter();
            p.SvmType = SvmType.EPSILON_SVR;
            p.Gamma = 0.05;
            p.C = 1024;
            p.Probability = true;
            var model = Training.Train(scaled, p);

            using (MemoryStream ms = new MemoryStream())
            {
                Model.Write(ms, model);
                ms.Position = 0;
                using (StreamReader sr = new StreamReader(ms))
                {
                    return sr.ReadToEnd();
                }
            }
        }

        private Node[][] ToNode(IList<double> features)
        {
            Node[] nodes = new Node[features.Count];
            for (int i = 0; i < features.Count; i++)
            {
                nodes[i] = new Node(i + 1, features[i]);
            }
            return new Node[1][] { nodes };
        }

        private string GetTrainingData(IList<double> features, float dmosScore)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(dmosScore + " ");
            for (int i = 0; i < features.Count; i++)
            {
                sb.Append(String.Format("{0}:{1} ", i + 1, ((float)features[i]).ToString().Replace(",", ".")));
            }
            sb.AppendLine();
            return sb.ToString();
        }

        private MemoryStream ToStream(string value)
        {
            return new MemoryStream(Encoding.UTF8.GetBytes(value ?? ""));
        }
    }
}