using System;
using System.IO;
using System.Text;
using Tesseract;

namespace ImageQuality
{
    public class SimpleWatermarkDetect
    {
        private SceneTextRegionExtractor _extractor;
        private TesseractEngine _ocr;

        public SimpleWatermarkDetect()
        {
            _extractor = new SceneTextRegionExtractor();
            _ocr = new TesseractEngine("./tessdata", "eng", EngineMode.Default);
        }

        public string Detect(byte[] fileBytes)
        {
            StringBuilder sb = new StringBuilder();

            using (var redDetect = new MemoryStream())
            {
                _extractor.Red(fileBytes, redDetect);
                sb.AppendLine(OcrImage(redDetect.ToArray()));
            }
            using (var whiteDetect = new MemoryStream())
            {
                _extractor.White(fileBytes, whiteDetect);
                sb.AppendLine(OcrImage(whiteDetect.ToArray()));
            }
            using (var blackDetect = new MemoryStream())
            {
                _extractor.Black(fileBytes, blackDetect);
                sb.AppendLine(OcrImage(blackDetect.ToArray()));
            }

            return sb.ToString();
        }

        private string OcrImage(byte[] ocrImg)
        {
            StringBuilder sb = new StringBuilder();

            var pix = Pix.LoadTiffFromMemory(ocrImg);

            using (var page = _ocr.Process(pix))
            {
                if (page.GetMeanConfidence() > 0.5)
                {
                    var text = page.GetText().Trim();
                    if (!String.IsNullOrEmpty(text))
                    {
                        sb.AppendLine(text);
                    }
                }
            }

            return sb.ToString();
        }
    }
}