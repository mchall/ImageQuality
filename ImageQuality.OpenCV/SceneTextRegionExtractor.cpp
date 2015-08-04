#include "Stdafx.h"
#include "SceneTextRegionExtractor.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	IList<GetRegionsResult^>^ SceneTextRegionExtractor::SimpleWatermark(array<byte>^ buffer)
	{
		Mat img = ReadImage(buffer);
		List<GetRegionsResult^>^ list = gcnew List<GetRegionsResult^>(4);

		GetRegionsResult^ red = DetectAndRotate(img, Scalar(0, 0, 200), Scalar(50, 50, 255));
		if (red != nullptr)
			list->Add(red);

		GetRegionsResult^ yellow = DetectAndRotate(img, Scalar(0, 225, 225), Scalar(50, 255, 255));
		if (yellow != nullptr)
			list->Add(yellow);

		GetRegionsResult^ white = DetectAndRotate(img, Scalar(200, 200, 200), Scalar(255, 255, 255));
		if (white != nullptr)
			list->Add(white);

		return list;
	}

	GetRegionsResult^ SceneTextRegionExtractor::DetectAndRotate(Mat img, Scalar lower, Scalar upper)
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

		return nullptr;
	}

	GetRegionsResult^ SceneTextRegionExtractor::GetRegions(array<byte>^ buffer)
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

				if (r > .45 && rect.width >= MinWidth / 2 && rect.height >= MinHeight)
				{
					Mat roi(tiff, rect);
					Mat local = roi.clone();

					if (HorizontalHeuristics(local))
					{
						regionRects.push_back(rect);
					}
					else
					{
						rectangle(image, rect, Scalar(0, 255, 255), 1);
					}
				}
			}

			if (!regionRects.empty())
			{
				vector<Rect> mergedRects = MergeRects(image.cols, image.rows, regionRects, 10); //letters
				mergedRects = MergeRects(image.cols, image.rows, mergedRects, 15); //words

				if (!mergedRects.empty())
				{
					for each (Rect rect in mergedRects)
					{
						Mat roi(tiff, rect);
						Mat local = roi.clone();

						if (rect.width >= MinWidth * 3 && rect.height > MinHeight && VerticalHeuristics(local))
						{
							rectangle(image, rect, Scalar(0, 255, 0), 2);

							//imshow("roi", roi);
							//waitKey(0);

							Region^ region = gcnew Region(rect, ToByteArray(local, ".tiff"));
							list->Add(region);
						}
						else
						{
							rectangle(image, rect, Scalar(0, 0, 255), 1);
						}
					}
				}
			}
		}

		imshow("debug", image);
		waitKey(0);

		GetRegionsResult^ result = gcnew GetRegionsResult(list, ToByteArray(image, ".jpg"));
		return result;
	}

	bool sortRects(Rect left, Rect right) 
	{ 
		return (left.x < right.x); 
	}

	vector<Rect> SceneTextRegionExtractor::MergeRects(int width, int height, vector<Rect> rects, int expand)
	{
		if (rects.empty())
			return rects;

		sort(rects.begin(), rects.end(), sortRects);

		List<int> indexes;
		vector<Rect> merged;
		for (int i = 0; i < rects.size(); i++)
		{
			if (!indexes.Contains(i))
			{
				Rect local = rects[i];
				for (int j = i + 1; j < rects.size(); j++)
				{
					Rect temp(local.x, local.y, local.width + expand, local.height);
					Rect temp2(rects[j].x - expand, rects[j].y, rects[j].width, rects[j].height);

					Rect interset = (temp & temp2);
					if (interset != temp && interset != temp2)
					{
						temp = Rect(local.x, local.y + local.height / 2 - 4, local.width + expand, 8);
						temp2 = Rect(rects[j].x - expand, rects[j].y + rects[j].height / 2 - 4, rects[j].width, 8);

						interset = (temp & temp2);
						if (interset.area() > 0)
						{
							local = local | rects[j];
							indexes.Add(j);
						}
					}
				}

				if (local.x + local.width > width)
				{
					local.width = width - local.x;
				}
				if (local.y + local.height > height)
				{
					local.height = height - local.y;
				}

				merged.push_back(local);
			}
		}

		return merged;
	}

	bool SceneTextRegionExtractor::HorizontalHeuristics(Mat roi)
	{
		if (NeedsInverse(roi))
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

		vector<vector<double>> horDivided = HeuristicSplit(horVals);
		if (horDivided.empty() || horDivided.size() > 2 || 
			horVals.empty() || horVals[0] > 0.05 || horVals[horVals.size() - 1] > 0.05)
		{
			/*Mat horMask = Mat(Size(100, horVals.size()), CV_8UC1, Scalar(255));
			for (int i = 0; i < horVals.size(); i++)
			{
				int x = (int)(horVals[i] * 100);
				for (int j = 0; j < x; j++)
				{
					horMask.at<uchar>(Point(j, i)) = 0;
				}
			}

			imshow("roi", roi);
			imshow("horizontal_mask", horMask);
			waitKey(0);*/

			return false;
		}

		return true;
	}

	bool SceneTextRegionExtractor::VerticalHeuristics(Mat roi)
	{
		if (NeedsInverse(roi))
		{
			bitwise_not(roi, roi);
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
			/*Mat vertMask = Mat(Size(verVals.size(), 100), CV_8UC1, Scalar(255));
			for (int i = 0; i < verVals.size(); i++)
			{
				int x = (int)(verVals[i] * 100);
				for (int j = 0; j < x; j++)
				{
					vertMask.at<uchar>(Point(i, 99 - j)) = 0;
				}
			}

			imshow("roi", roi);
			imshow("vertical_mask", vertMask);
			waitKey(0);*/

			return false;
		}

		Mat clone = roi.clone();
		bitwise_not(clone, clone);

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(clone, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		int sum = 0;
		double total = 0;
		vector<int> yPoints;

		if (hierarchy.empty())
			return false;

		for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
		{
			Rect br = boundingRect(contours[idx]);
			if (br.height > 2 && br.width > 2)
			{
				int midPt = br.y + br.height / 2;
				sum += midPt;
				total++;
				yPoints.push_back(midPt);

				if (br.area() > roi.size().area() * 0.5)
				{
					return false;
				}
			}
		}

		double avg = sum / total;
		int outsideCount = 0;
		for each (int pt in yPoints)
		{
			if (pt > avg + 10 || pt < avg - 10)
			{
				outsideCount++;
			}
		}

		double outsidePercentage = outsideCount / total;
		if (outsidePercentage > 0.4)
		{
			return false;
		}

		return true;
	}

	bool SceneTextRegionExtractor::NeedsInverse(Mat roi)
	{
		int v1 = roi.at<uchar>(0, 0);
		int v2 = roi.at<uchar>(roi.rows - 1, roi.cols - 1);
		int v3 = roi.at<uchar>(0, roi.cols - 1);
		int v4 = roi.at<uchar>(roi.rows - 1, 0);

		if ((v1 + v2 + v3 + v4) / 4 < 128)
		{
			return true;
		}
		return false;
	}

	vector<vector<double>> SceneTextRegionExtractor::HeuristicSplit(vector<double> values)
	{
		vector<vector<double>> divided;

		if (values.empty())
			return divided;

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

		if (divided.empty() && !current.empty())
			divided.push_back(values);

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