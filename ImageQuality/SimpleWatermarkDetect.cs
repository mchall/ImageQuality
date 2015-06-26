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

            using (var red = new MemoryStream())
            using (var yellow = new MemoryStream())
            using (var white = new MemoryStream())
            using (var black = new MemoryStream())
            {
                _extractor.SimpleWatermark(fileBytes, red, yellow, black, white);
                OcrImage(sb, red.ToArray());
                OcrImage(sb, yellow.ToArray());
                OcrImage(sb, black.ToArray());
                OcrImage(sb, white.ToArray());
            }

            return sb.ToString();
        }

        private void OcrImage(StringBuilder sb, byte[] ocrImg)
        {
            var pix = Pix.LoadTiffFromMemory(ocrImg);

            using (var page = OcrEngine.Instance.Process(pix))
            {
                if (page.GetMeanConfidence() > OcrEngine.MinConfidence)
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