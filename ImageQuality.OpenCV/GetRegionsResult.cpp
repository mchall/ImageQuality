#include "Stdafx.h"
#include "GetRegionsResult.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	GetRegionsResult::GetRegionsResult(IList<Region^>^ regions, cli::array<byte>^ debug)
	{
		_regions = regions;
		_debug = debug;
	}

	IList<Region^>^ GetRegionsResult::Regions::get()
	{
		return _regions;
	}

	cli::array<byte>^ GetRegionsResult::Debug::get()
	{
		return _debug;
	}
}