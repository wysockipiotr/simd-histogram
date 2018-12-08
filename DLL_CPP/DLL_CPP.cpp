#include "stdafx.h"
#include "DLL_CPP.h"
#include <limits>

// block max macro
#ifdef max
#undef max
extern "C" unsigned __int32 MAX_NUMBER_OF_PIXELS =
	std::numeric_limits<unsigned __int32>::max();
#endif

// TODO Cpp implementation goes here

extern "C" DLLCPP_API bool _cpp_calculate_histogram (
	unsigned __int32* histogram,
	unsigned __int8* pixel_buffer,
	unsigned __int32 number_of_pixels) { return true; }
