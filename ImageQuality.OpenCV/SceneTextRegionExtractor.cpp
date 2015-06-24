#include "Stdafx.h"
#include "SceneTextRegionExtractor.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	IList<Region^>^ SceneTextRegionExtractor::GetRegions(array<unsigned char>^ buffer)
	{
		pin_ptr<unsigned char> px = &buffer[0];
		cv::Mat datax(1, buffer->Length, CV_8U, (void*)px, CV_AUTO_STEP);
		cv::Mat large = imdecode(datax, CV_LOAD_IMAGE_COLOR);

		Mat rgb;
		pyrDown(large, rgb);
		Mat small;
		cvtColor(rgb, small, CV_BGR2GRAY);

		Mat grad;
		Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);

		Mat bw;
		threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);

		Mat connected;
		morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
		morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);

		Mat mask = Mat::zeros(bw.size(), CV_8UC1);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		List<Region^>^ list = gcnew List<Region^>(5);
		for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
		{
			Rect rect = boundingRect(contours[idx]);
			Mat maskROI(mask, rect);
			maskROI = Scalar(0, 0, 0);

			drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
			double r = (double)countNonZero(maskROI) / (rect.width*rect.height);

			if (r > .45 && rect.height > 8 && rect.width > 8)
			{
				//rectangle(rgb, rect, Scalar(0, 255, 0), 2);

				Region^ region = gcnew Region(rect.x * 2, rect.y * 2, rect.width * 2, rect.height * 2);
				list->Add(region);
			}
		}

		//imshow("rgb", rgb);
		return list;		
	}
}