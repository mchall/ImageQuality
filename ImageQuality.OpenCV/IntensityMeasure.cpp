#include "Stdafx.h"
#include "IntensityMeasure.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	double IntensityMeasure::IntensityTest(array<byte>^ buffer)
	{
		Mat image = ReadImage(buffer);
		return countNonZero(image > 100) / (double)(image.rows * image.cols);
	}

	Mat IntensityMeasure::ReadImage(array<byte>^ buffer)
	{
		pin_ptr<byte> px = &buffer[0];
		Mat datax(1, buffer->Length, CV_8U, (void*)px, CV_AUTO_STEP);
		return imdecode(datax, CV_LOAD_IMAGE_GRAYSCALE);
	}
}