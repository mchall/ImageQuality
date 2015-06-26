#include "Stdafx.h"
#include "SceneTextRegionExtractor.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	void SceneTextRegionExtractor::SimpleWatermark(array<unsigned char>^ buffer, Stream^ redStream, Stream^ yellowStream, Stream^ blackStream, Stream^ whiteStream)
	{
		Mat img = ReadImage(buffer);

		if (redStream != nullptr)
		{
			Mat red = DetectAndRotate(img, Scalar(0, 0, 200), Scalar(50, 50, 255));
			WriteToStream(".tiff", red, redStream);
		}

		if (yellowStream != nullptr)
		{
			Mat yellow = DetectAndRotate(img, Scalar(0, 225, 225), Scalar(50, 255, 255));
			WriteToStream(".tiff", yellow, yellowStream);
		}

		if (blackStream != nullptr)
		{
			Mat black = DetectAndRotate(img, Scalar(0, 0, 0), Scalar(25, 25, 25));
			WriteToStream(".tiff", black, blackStream);
		}

		if (whiteStream != nullptr)
		{
			Mat white = DetectAndRotate(img, Scalar(225, 225, 225), Scalar(255, 255, 255));
			WriteToStream(".tiff", white, whiteStream);
		}
	}

	Mat SceneTextRegionExtractor::DetectAndRotate(Mat img, Scalar lower, Scalar upper)
	{
		Mat output;
		inRange(img, lower, upper, output);

		Mat connected;
		pyrDown(output, connected);
		Mat morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
		morphologyEx(connected, connected, MORPH_CLOSE, morphKernel);

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		if (hierarchy.empty())
			return output;

		float biggest = 0, angle;
		for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
		{
			RotatedRect box = minAreaRect(contours[idx]);
			
			float area = box.size.area();
			if (area > biggest && box.size.width > 8 && box.size.height > 8)
			{
				biggest = area;
				angle = box.angle;
			}
		}

		if (angle < -45)
			angle += 90;

		int len = max(output.cols, output.rows);
		Point2f pt(len / 2., len / 2.);
		Mat r = getRotationMatrix2D(pt, angle, 1);
		warpAffine(output, output, r, Size(len, len));

		return output;
	}

	IList<Region^>^ SceneTextRegionExtractor::GetRegions(array<unsigned char>^ buffer, Stream^ ocrImgStream, Stream^ regionStream)
	{
		Mat image = ReadImage(buffer);

		if (ocrImgStream != nullptr)
		{
			Mat tiff;
			cvtColor(image, tiff, CV_BGR2GRAY);
			threshold(tiff, tiff, 0, 255, THRESH_BINARY | THRESH_OTSU);
			WriteToStream(".tiff", tiff, ocrImgStream);
		}

		Mat rgb;
		pyrDown(image, rgb);
		Mat small;
		cvtColor(rgb, small, CV_BGR2GRAY);

		Mat grad;
		Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);

		Mat bw;
		threshold(grad, bw, 0, 255, THRESH_BINARY | THRESH_OTSU);

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
				rectangle(rgb, rect, Scalar(0, 255, 0), 2);

				Region^ region = gcnew Region(rect.x * 2, rect.y * 2, rect.width * 2, rect.height * 2);
				list->Add(region);
			}
		}

		if (regionStream != nullptr)
		{
			WriteToStream(".jpg", rgb, regionStream);
		}

		return list;
	}

	Mat SceneTextRegionExtractor::ReadImage(array<unsigned char>^ buffer)
	{
		pin_ptr<unsigned char> px = &buffer[0];
		Mat datax(1, buffer->Length, CV_8U, (void*)px, CV_AUTO_STEP);
		return imdecode(datax, CV_LOAD_IMAGE_COLOR);
	}

	void SceneTextRegionExtractor::WriteToStream(const std::string& extension, Mat image, Stream^ stream)
	{
		vector<uchar> buffer;
		imencode(extension, image, buffer);

		UnmanagedMemoryStream^ sourceStream = gcnew UnmanagedMemoryStream(buffer.data(), buffer.size());
		sourceStream->CopyTo(stream);
	}
}