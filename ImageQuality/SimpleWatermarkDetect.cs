using System;
using System.IO;
using System.Text;
using Tesseract;

namespace ImageQuality
{
    public class SimpleWatermarkDetect
    {
        private SceneTextRegionExtractor _extractor;

        public SimpleWatermarkDetect()
        {
            _extractor = new SceneTextRegionExtractor();
        }

        public string Detect(byte[] fileBytes)
        {
            StringBuilder sb = new StringBuilder();

            foreach (var bytes in _extractor.SimpleWatermark(fileBytes))
            {
                OcrImage(sb, bytes);
            }

            return sb.ToString();
        }

        private void OcrImage(StringBuilder sb, byte[] ocrImg)
        {
            var pix = Pix.LoadTiffFromMemory(ocrImg);

            using (var page = OcrEngine.Instance.Process(pix))
            {
                if (page.GetMeanConfidence() >= OcrEngine.MinConfidence)
                {
                    var text = page.GetText().Trim();
                    if (!String.IsNullOrEmpty(text))
                    {
                        sb.AppendLine(text);
                    }
                }
            }
        }
    }
}