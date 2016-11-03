#include "Stdafx.h"
#include "ColorMeasure.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	ColorInfo^ ColorMeasure::GetColorInfo(cli::array<byte>^ buffer)
	{
		Mat image = ReadImage(buffer);

		vector<Mat> channels;
		split(image, channels);

		Mat saturation;
		inRange(channels[1], 50, 255, saturation);

		Mat red, blue, green, yellow;
		inRange(channels[0], 0, 10, red);
		inRange(channels[0], 35, 75, green);
		inRange(channels[0], 90, 125, blue);
		inRange(channels[0], 16, 21, yellow);

		bitwise_and(saturation, red, red);
		bitwise_and(saturation, green, green);
		bitwise_and(saturation, blue, blue);
		bitwise_and(saturation, yellow, yellow);

		double image_size = image.cols * image.rows;
		double r_percent = ((double)cv::countNonZero(red)) / image_size;
		double g_percent = ((double)cv::countNonZero(green)) / image_size;
		double b_percent = ((double)cv::countNonZero(blue)) / image_size;
		double y_percent = ((double)cv::countNonZero(yellow)) / image_size;

		return gcnew ColorInfo(r_percent, g_percent, b_percent, y_percent);
	}

	Mat ColorMeasure::ReadImage(cli::array<byte>^ buffer)
	{
		pin_ptr<byte> px = &buffer[0];
		Mat datax(1, buffer->Length, CV_8U, (void*)px, CV_AUTO_STEP);
		Mat img = imdecode(datax, CV_LOAD_IMAGE_COLOR);
		cvtColor(img, img, CV_BGR2HSV);
		return img;
	}
}