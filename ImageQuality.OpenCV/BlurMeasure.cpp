#include "Stdafx.h"
#include "BlurMeasure.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	double BlurMeasure::BlurTest(cli::array<byte>^ buffer)
	{
		Mat image = ReadImage(buffer);
		resize(image, image, Size(512, 512));

		Mat gray;
		cvtColor(image, gray, CV_BGR2GRAY);

		Mat laplacian;
		Laplacian(gray, laplacian, CV_64F);

		Mat mean, stddev;
		meanStdDev(laplacian, mean, stddev);
		double val = stddev.at<double>(0, 0);

		return val * val;
	}

	Mat BlurMeasure::ReadImage(cli::array<byte>^ buffer)
	{
		pin_ptr<byte> px = &buffer[0];
		Mat datax(1, buffer->Length, CV_8U, (void*)px, CV_AUTO_STEP);
		return imdecode(datax, cv::IMREAD_COLOR);
	}
}