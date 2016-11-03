#pragma once

#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include "Region.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace std;
using namespace cv;

namespace ImageQuality {

	public ref class GetRegionsResult
	{
	private:
		IList<Region^>^ _regions;
		cli::array<byte>^ _debug;

	internal:
		GetRegionsResult(IList<Region^>^ regions, cli::array<byte>^ debug);

	public:
		property IList<Region^>^ Regions { IList<Region^>^ get(); }
		property cli::array<byte>^ Debug { cli::array<byte>^ get(); }
	};
}