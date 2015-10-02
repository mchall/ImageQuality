#include "Stdafx.h"
#include "IntensityMeasure.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	double IntensityMeasure::IntensityTest(array<byte>^ buffer)
	{
		Mat image = ReadImage(buffer);
		/*Mat hist;

		int channels[] = { 0 };
		int histSize[] = { 32 };
		float range[] = { 0, 256 };
		const float* ranges[] = { range };

		calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);*/

		Mat intensityMat = image > 100;
		return countNonZero(intensityMat) / (double)(image.rows * image.cols);
	}

	Mat IntensityMeasure::ReadImage(array<byte>^ buffer)
	{
		pin_ptr<byte> px = &buffer[0];
		Mat datax(1, buffer->Length, CV_8U, (void*)px, CV_AUTO_STEP);
		return imdecode(datax, CV_LOAD_IMAGE_GRAYSCALE);
	}
}