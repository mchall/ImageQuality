#include "Stdafx.h"
#include "BlurMeasure.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	double BlurMeasure::BlurTest(array<byte>^ buffer)
	{
		Mat image = ReadImage(buffer);
		resize(image, image, Size(512, 512));

		Mat gray;
		cvtColor(image, gray, CV_BGR2GRAY);

		Mat detected_edges;
		Canny(gray, detected_edges, 0.4, 0.4 * 3, 3);

		double maxval;
		double average = mean(detected_edges)[0];
		int* const maxIdx = (int*)malloc(sizeof(detected_edges));

		minMaxIdx(detected_edges, 0, &maxval, 0, maxIdx);

		double blurresult = average / maxval;
		return blurresult;
	}

	Mat BlurMeasure::ReadImage(array<byte>^ buffer)
	{
		pin_ptr<byte> px = &buffer[0];
		Mat datax(1, buffer->Length, CV_8U, (void*)px, CV_AUTO_STEP);
		return imdecode(datax, CV_LOAD_IMAGE_COLOR);
	}
}