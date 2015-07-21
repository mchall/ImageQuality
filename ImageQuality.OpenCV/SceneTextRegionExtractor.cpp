#include "Stdafx.h"
#include "SceneTextRegionExtractor.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	IList<Region^>^ SceneTextRegionExtractor::SimpleWatermark(array<byte>^ buffer)
	{
		Mat img = ReadImage(buffer);
		List<Region^>^ list = gcnew List<Region^>(5);

		IList<Region^>^ red = DetectAndRotate(img, Scalar(0, 0, 200), Scalar(50, 50, 255));
		list->AddRange(red);

		IList<Region^>^ yellow = DetectAndRotate(img, Scalar(0, 225, 225), Scalar(50, 255, 255));
		list->AddRange(yellow);

		IList<Region^>^ black = DetectAndRotate(img, Scalar(0, 0, 0), Scalar(25, 25, 25));
		list->AddRange(black);

		IList<Region^>^ white = DetectAndRotate(img, Scalar(200, 200, 200), Scalar(255, 255, 255));
		list->AddRange(white);

		return list;
	}

	IList<Region^>^ SceneTextRegionExtractor::DetectAndRotate(Mat img, Scalar lower, Scalar upper)
	{
		Mat output;
		inRange(img, lower, upper, output);

		Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(2, 2));
		morphologyEx(output, output, MORPH_CLOSE, morphKernel);

		Mat contourImg = output.clone();

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(contourImg, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		if (!hierarchy.empty())
		{
			vector<float> angles;
			for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
			{
				RotatedRect box = minAreaRect(contours[idx]);

				float area = box.size.area();
				if (box.size.width > MinWidth && box.size.height > MinHeight)
				{
					float angle = box.angle;
					if (angle < -45)
						angle += 90;
					angles.push_back(angle);
				}
			}

			if (!angles.empty())
			{
				Nullable<float> bestAngle = FindBestAngle(angles);
				if (bestAngle.HasValue)
				{
					float angle = (bestAngle.Value > -18 && bestAngle.Value < 18) ? 0 : bestAngle.Value;

					int len = max(output.cols, output.rows);
					Point2f pt(len / 2., len / 2.);
					Mat r = getRotationMatrix2D(pt, angle, 1);
					warpAffine(output, output, r, Size(len, len));

					cvtColor(output, output, CV_GRAY2BGR);

					array<byte>^ b = ToByteArray(output, ".tiff");
					return GetRegions(b);
				}
			}
		}

		return gcnew List<Region^>(0);
	}
		
	IList<Region^>^ SceneTextRegionExtractor::GetRegions(array<byte>^ buffer)
	{
		List<Region^>^ list = gcnew List<Region^>(5);
		Mat image = ReadImage(buffer);

		Mat tiff;
		cvtColor(image, tiff, CV_BGR2GRAY);
		threshold(tiff, tiff, 0, 255, THRESH_BINARY | THRESH_OTSU);

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
		morphKernel = getStructuringElement(MORPH_RECT, Size(8, 1));
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

				if (r > .45 && rect.width >= 10 && rect.height >= 12)
				{
					regionRects.push_back(rect);
				}
			}

			if (!regionRects.empty())
			{
				Mat rectMask(Size(gray.cols, gray.rows), gray.type(), Scalar(0, 0, 0));
				for each (Rect rect in regionRects)
				{
					Rect expanded(rect.x - 10, rect.y, rect.width + 20, rect.height);
					rectangle(rectMask, expanded, Scalar(255, 255, 255), 2);
				}

				findContours(rectMask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

				vector<Rect> mergedRects;
				for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
				{
					Rect rect = boundingRect(contours[idx]);
					Rect tighter(rect.x + 5, rect.y - 1, rect.width - 10, rect.height + 2);
					if (tighter.width >= MinWidth && tighter.height >= MinHeight)
					{
						mergedRects.push_back(tighter);
					}
				}

				if (!mergedRects.empty())
				{
					for each (Rect rect in mergedRects)
					{
						Mat roi(tiff, rect);
						Mat local = roi.clone();

						if (HeuristicEvaluation(local))
						{
							rectangle(image, rect, Scalar(0, 255, 0), 2);

							//imshow("roi", roi);
							//waitKey(0);

							Region^ region = gcnew Region(rect, ToByteArray(local, ".tiff"));
							list->Add(region);
						}
						else
						{
							rectangle(image, rect, Scalar(0, 0, 255), 2);
						}
					}
				}
			}
		}

		//imshow("debug", image);
		//waitKey(0);

		return list;
	}

	bool SceneTextRegionExtractor::HeuristicEvaluation(Mat roi)
	{
		double totC = countNonZero(roi) / (double)(roi.cols * roi.rows);
		if (totC < 0.5)
		{
			bitwise_not(roi, roi);
		}

		vector<double> horVals;
		for (int x = 0; x < roi.rows; x++)
		{
			Mat row = roi.row(x);
			double c = (roi.cols - countNonZero(row)) / (double)roi.cols;
			horVals.push_back(c);
		}

		if (horVals.empty() || horVals[0] != 0 || horVals[horVals.size() - 1] != 0)
		{
			return false;
		}

		vector<double> verVals;
		for (int x = 0; x < roi.cols; x++)
		{
			Mat col = roi.col(x);
			double c = (roi.rows - countNonZero(col)) / (double)roi.rows;
			verVals.push_back(c);
		}

		vector<vector<double>> vertDivided = HeuristicSplit(verVals);

		if (vertDivided.empty() || vertDivided.size() < 2) //Text has spaces between chars
		{
			return false;
		}

		/*Mat vertMask = Mat(Size(verVals.size(), 100), CV_8UC1, Scalar(255));
		for (int i = 0; i < verVals.size(); i++)
		{
			int x = (int)(verVals[i] * 100);
			for (int j = 0; j < x; j++)
			{
				vertMask.at<uchar>(Point(i, 99 - j)) = 0;
			}
		}

		Mat horMask = Mat(Size(100, horVals.size()), CV_8UC1, Scalar(255));
		for (int i = 0; i < horVals.size(); i++)
		{
			int x = (int)(horVals[i] * 100);
			for (int j = 0; j < x; j++)
			{
				horMask.at<uchar>(Point(j, i)) = 0;
			}
		}

		imshow("roi", roi);
		imshow("vertical_mask", vertMask);
		imshow("horizontal_mask", horMask);
		waitKey(0);*/
		
		return true;
	}

	vector<vector<double>> SceneTextRegionExtractor::HeuristicSplit(vector<double> values)
	{
		vector<vector<double>> divided;
		vector<double> current;
		for (int i = 0; i < values.size(); i++)
		{
			if (values[i] == 0)
			{
				if (!current.empty())
				{
					divided.push_back(current);
					current.clear();
				}
			}
			else
			{
				current.push_back(values[i]);
			}
		}
		return divided;
	}

	Nullable<float> SceneTextRegionExtractor::FindBestAngle(vector<float> angles)
	{
		float bestAngle;
		int bestAngleCount = 1;
		bool found = false;

		if (!angles.empty())
		{
			for (int i = 0; i < angles.size() - 1; i++)
			{
				float angle = angles[i];
				int count = 1;
				for (int j = i + 1; j < angles.size(); j++)
				{
					if (i == j) continue;

					if (abs(angles[i] - angles[j]) < 10)
					{
						if (std::abs(angles[j]) < std::abs(angles[i]))
						{
							angle = angles[j];
						}
						count++;
					}
				}

				if (count > bestAngleCount && count >= angles.size() * 0.6)
				{
					bestAngle = angle;
					bestAngleCount = count;
					found = true;
				}
			}
		}
		return found ? bestAngle : Nullable<float>();
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