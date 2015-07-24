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
            List<Region> regions = new List<Region>();
            foreach (var result in _extractor.SimpleWatermark(fileBytes))
            {
                regions.AddRange(result.Regions);
            }
            return Ocr(regions, 0.6f);
        }

        public string NaturalSceneDetect(byte[] fileBytes, out byte[] debugBytes)
        {
            var result = _extractor.GetRegions(fileBytes);
            debugBytes = result.Debug;
            return Ocr(result.Regions, 0.7f);
        }

        private string Ocr(IList<Region> regions, float minConfidence)
        {
            StringBuilder sb = new StringBuilder();
            foreach (var region in OrderRegions(regions))
            {
                using (var pix = Pix.LoadTiffFromMemory(region.Tiff))
                {
                    using (var page = OcrEngine.Instance.Process(pix, PageSegMode.SingleLine))
                    {
                        var confidence = page.GetMeanConfidence();
                        //pix.Save(DateTime.Now.Ticks + "_" + Math.Round(confidence * 100) + ".tiff");

                        if (confidence >= minConfidence)
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