#include "Stdafx.h"
#include "BlurMeasure.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	double BlurMeasure::Nayar89(array<unsigned char>^ buffer)
	{
		Mat image = ReadImage(buffer);

		Mat lap;
		Laplacian(image, lap, CV_64F);

		Scalar mu, sigma;
		meanStdDev(lap, mu, sigma);

		return sigma.val[0] * sigma.val[0];
	}

	double BlurMeasure::Pech2000(array<unsigned char>^ buffer)
	{
		Mat image = ReadImage(buffer);

		Mat lap;
		Laplacian(image, lap, CV_64F);

		Scalar mu, sigma;
		meanStdDev(lap, mu, sigma);

		return sigma.val[0] * sigma.val[0];
	}

	Mat BlurMeasure::ReadImage(array<unsigned char>^ buffer)
	{
		pin_ptr<unsigned char> px = &buffer[0];
		Mat datax(1, buffer->Length, CV_8U, (void*)px, CV_AUTO_STEP);
		return imdecode(datax, CV_LOAD_IMAGE_COLOR);
	}
}