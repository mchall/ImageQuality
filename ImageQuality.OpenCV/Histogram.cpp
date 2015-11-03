#include "Stdafx.h"
#include "Histogram.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	double Histogram::Compare(array<byte>^ left, array<byte>^ right)
	{
		Mat l = ReadImage(left);
		Mat r = ReadImage(right);

		cvtColor(l, l, CV_BGR2RGB);
		cvtColor(r, r, CV_BGR2RGB);

		Mat histL;
		Mat histR;

		int nimages = 1;
		int channels[] = { 0, 1, 2 };
		int dims = 1;
		int histSize[] = { 8, 8, 8 };
		float hranges[] = { 0, 256, 0, 256, 0, 256 };
		const float *ranges[] = { hranges };

		calcHist(&l, nimages, channels, Mat(), histL, dims, histSize, ranges, true);
		normalize(histL, histL);
		Mat flatLeft = histL.reshape(1, 1);

		calcHist(&r, nimages, channels, Mat(), histR, dims, histSize, ranges, true);
		normalize(histR, histR);
		Mat flatRight = histR.reshape(1, 1);

		return compareHist(flatLeft, flatRight, CV_COMP_CHISQR);
	}

	Mat Histogram::ReadImage(array<byte>^ buffer)
	{
		pin_ptr<byte> px = &buffer[0];
		Mat datax(1, buffer->Length, CV_8U, (void*)px, CV_AUTO_STEP);
		return imdecode(datax, CV_LOAD_IMAGE_COLOR);
	}
}