#include "Stdafx.h"
#include "BlurMeasure.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	double BlurMeasure::Measure(array<unsigned char>^ buffer)
	{
		pin_ptr<unsigned char> px = &buffer[0];
		Mat datax(1, buffer->Length, CV_8U, (void*)px, CV_AUTO_STEP);
		Mat image = imdecode(datax, CV_LOAD_IMAGE_COLOR);

		//tmp
		//resize(image, image, Size(508, 373));

		Mat lap;
		Laplacian(image, lap, CV_64F);

		Scalar mu, sigma;
		meanStdDev(lap, mu, sigma);

		double focusMeasure = sigma.val[0] * sigma.val[0];
		return focusMeasure;
	}
}