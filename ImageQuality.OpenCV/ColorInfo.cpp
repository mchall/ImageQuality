#include "Stdafx.h"
#include "ColorInfo.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	ColorInfo::ColorInfo(double r, double g, double b)
	{
		_r = r;
		_g = g;
		_b = b;
	}

	double ColorInfo::R::get()
	{
		return _r;
	}

	double ColorInfo::G::get()
	{
		return _g;
	}

	double ColorInfo::B::get()
	{
		return _b;
	}
}