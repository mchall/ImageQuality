#include "Stdafx.h"
#include "SceneTextRegionExtractor.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	IList<array<byte>^>^ SceneTextRegionExtractor::SimpleWatermark(array<byte>^ buffer)
	{
		Mat img = ReadImage(buffer);

		List<array<byte>^>^ images = gcnew List<array<byte>^>(4);

		Mat red = DetectAndRotate(img, Scalar(0, 0, 200), Scalar(50, 50, 255));
		images->Add(ToByteArray(red, ".tiff"));

		Mat yellow = DetectAndRotate(img, Scalar(0, 225, 225), Scalar(50, 255, 255));
		images->Add(ToByteArray(yellow, ".tiff"));

		Mat black = DetectAndRotate(img, Scalar(0, 0, 0), Scalar(25, 25, 25));
		images->Add(ToByteArray(black, ".tiff"));

		Mat white = DetectAndRotate(img, Scalar(200, 200, 200), Scalar(255, 255, 255));
		images->Add(ToByteArray(white, ".tiff"));

		return images;
	}

	Mat SceneTextRegionExtractor::DetectAndRotate(Mat img, Scalar lower, Scalar upper)
	{
		Mat output;
		inRange(img, lower, upper, output);

		Mat scaled;
		pyrDown(output, scaled);

		Mat morphKernel = getStructuringElement(MORPH_RECT, Size(5, 1));
		morphologyEx(scaled, scaled, MORPH_CLOSE, morphKernel);

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(scaled, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		if (hierarchy.empty())
			return output;

		float biggest = 0, angle;
		for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
		{
			RotatedRect box = minAreaRect(contours[idx]);

			float area = box.size.area();
			if (area > biggest && box.size.width > 20 && box.size.height > 12)
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

	IList<Region^>^ SceneTextRegionExtractor::GetRegions(array<byte>^ buffer, Stream^ ocrImgStream, Stream^ regionStream)
	{
		List<Region^>^ list = gcnew List<Region^>(5);
		Mat image = ReadImage(buffer);

		if (ocrImgStream != nullptr)
		{
			Mat tiff;
			cvtColor(image, tiff, CV_BGR2GRAY);
			threshold(tiff, tiff, 0, 255, THRESH_BINARY | THRESH_OTSU);

			//threshold(tiff, tiff, 190, 255, THRESH_BINARY_INV); //todo

			WriteToStream(".tiff", tiff, ocrImgStream);
		}

		Mat gray;
		cvtColor(image, gray, CV_BGR2GRAY);

		Mat scaled = gray;
		pyrDown(scaled, scaled);
		pyrUp(scaled, scaled);

		Mat grad;
		Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		morphologyEx(scaled, grad, MORPH_GRADIENT, morphKernel);

		Mat bw;
		threshold(grad, bw, 32, 255, THRESH_BINARY | THRESH_OTSU);

		Mat connected;
		morphKernel = getStructuringElement(MORPH_RECT, Size(5, 1));
		morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);

		Mat mask = Mat::zeros(bw.size(), CV_8UC1);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		if (!hierarchy.empty())
		{
			vector<Rect> regionRects;
			for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
			{
				Rect rect = boundingRect(contours[idx]);
				if (rect.height > 400)
					continue;

				Mat maskROI(mask, rect);
				maskROI = Scalar(0, 0, 0);

				drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
				double r = (double)countNonZero(maskROI) / (rect.width*rect.height);

				if (r > .45 && rect.width > 20 && rect.height > 12)
				{
					regionRects.push_back(rect);
				}
			}

			if (!regionRects.empty())
			{
				Mat rectMask(Size(gray.cols, gray.rows), gray.type(), Scalar(0, 0, 0));
				for each (Rect rect in regionRects)
				{
					Rect expanded(rect.x - 5, rect.y, rect.width + 10, rect.height);
					rectangle(rectMask, expanded, Scalar(255, 255, 255), 2);
				}

				findContours(rectMask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

				vector<Rect> mergedRects;
				for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
				{
					Rect rect = boundingRect(contours[idx]);
					mergedRects.push_back(rect);
				}

				if (!mergedRects.empty())
				{
					Mat edges;
					Canny(scaled, edges, 128.0, 200.0, 3);

					Mat bin_edges;
					threshold(edges, bin_edges, 32, 255, THRESH_BINARY | THRESH_OTSU);

					for each (Rect rect in mergedRects)
					{
						Mat edgesROI(bin_edges, rect);

						vector<vector<Point> > subContours;
						vector<Vec4i> subHierarchy;
						findContours(edgesROI, subContours, subHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

						int linkCount = 0;

						for (int i = 0; i < subContours.size(); ++i)
						{
							Rect a_r = boundingRect(subContours[i]);
							double a_x = a_r.x + a_r.width / 2;
							double a_y = a_r.y + a_r.height / 2;

							for (int j = 0; j < subContours.size(); ++j)
							{
								if (j == i){ continue; }

								Rect b_r = boundingRect(subContours[j]);
								double b_x = b_r.x + b_r.width / 2;
								double b_y = b_r.y + b_r.height / 2;
								if (a_r.x < b_r.x + b_r.width && a_r.x + a_r.width > b_r.x &&
									a_r.y < b_r.y + b_r.width && a_r.y + a_r.width > b_r.y)
								{
									continue;
								}

								double a_min = std::min(a_r.width, a_r.height);
								double b_min = std::min(b_r.width, b_r.height);
								if (a_min > 1.5*b_min || b_min > 1.5*a_min){ continue; }

								double d_x = a_x - b_x;
								double d_y = a_y - b_y;
								double d = sqrt(d_x*d_x + d_y*d_y);

								if (d < (1.5 * 0.5 * (a_min + b_min)))
								{
									linkCount++;
								}
							}
						}

						if (linkCount > 1)
						{
							rectangle(image, rect, Scalar(0, 255, 0), 2);

							Region^ region = gcnew Region(rect.x, rect.y, rect.width, rect.height);
							list->Add(region);
						}
					}
				}
			}
		}

		if (regionStream != nullptr)
		{
			WriteToStream(".jpg", image, regionStream);
		}

		return list;
	}

	Mat SceneTextRegionExtractor::ReadImage(array<byte>^ buffer)
	{
		pin_ptr<byte> px = &buffer[0];
		Mat datax(1, buffer->Length, CV_8U, (void*)px, CV_AUTO_STEP);
		return imdecode(datax, CV_LOAD_IMAGE_COLOR);
	}

	array<byte>^ SceneTextRegionExtractor::ToByteArray(Mat image, const std::string& extension)
	{
		vector<byte> buffer;
		imencode(extension, image, buffer);

		array<byte>^ bytes = gcnew array<byte>(buffer.size());

		UnmanagedMemoryStream^ sourceStream = gcnew UnmanagedMemoryStream(buffer.data(), buffer.size());
		sourceStream->Read(bytes, 0, buffer.size());

		return bytes;
	}

	void SceneTextRegionExtractor::WriteToStream(const std::string& extension, Mat image, Stream^ stream)
	{
		vector<byte> buffer;
		imencode(extension, image, buffer);

		UnmanagedMemoryStream^ sourceStream = gcnew UnmanagedMemoryStream(buffer.data(), buffer.size());
		sourceStream->CopyTo(stream);
	}
}