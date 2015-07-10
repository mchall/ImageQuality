using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using Tesseract;

namespace ImageQuality
{
    public class ImageTextExtractor
    {
        private SceneTextRegionExtractor _extractor;

        public ImageTextExtractor()
        {
            _extractor = new SceneTextRegionExtractor();
        }

        public string WatermarkDetect(byte[] fileBytes)
        {
            StringBuilder sb = new StringBuilder();

            var regions = _extractor.SimpleWatermark(fileBytes);
            foreach (var region in OrderRegions(regions))
            {
                using (var pix = Pix.LoadTiffFromMemory(region.Tiff))
                {
                    using (var page = OcrEngine.Instance.Process(pix))
                    {
                        if (page.GetMeanConfidence() >= OcrEngine.MinConfidence)
                        {
                            EvaluateText(page.GetText(), sb);
                        }
                    }
                }
            }

            return sb.ToString();
        }

        public string NaturalSceneDetect(byte[] fileBytes)
        {
            StringBuilder sb = new StringBuilder();

            var regions = _extractor.GetRegions(fileBytes);
            foreach (var region in OrderRegions(regions))
            {
                using (var pix = Pix.LoadTiffFromMemory(region.Tiff))
                {
                    using (var page = OcrEngine.Instance.Process(pix))
                    {
                        if (page.GetMeanConfidence() >= OcrEngine.MinConfidence)
                        {
                            EvaluateText(page.GetText(), sb);
                        }
                    }
                }
            }

            return sb.ToString();
        }

        private List<Region> OrderRegions(IList<Region> regions)
        {
            var sort = regions.ToList();
            sort.Sort((l, r) =>
                {
                    if (Math.Abs(l.Y - r.Y) < 15)
                        return l.X.CompareTo(r.X);
                    return l.Y.CompareTo(r.Y);
                });
            return sort;
        }

        private void EvaluateText(string ocrText, StringBuilder sb)
        {
            if (ocrText != null)
            {
                var text = ocrText.Trim();
                Regex rgx = new Regex("[^a-zA-Z0-9 ]");
                text = rgx.Replace(text, " ");
                text = text.Trim();

                if (!String.IsNullOrEmpty(text) && text.Length > 2)
                {
                    sb.AppendLine(text);
                }
            }
        }
    }
}