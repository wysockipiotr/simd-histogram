#include "stdafx.h"
#include "DLL_CPP.h"
#include <limits>

// block max macro
#ifdef max
#undef max
extern "C" unsigned __int32 MAX_NUMBER_OF_PIXELS =
	std::numeric_limits<unsigned __int32>::max();
#endif

extern "C" DLLCPP_API bool _cpp_calculate_histogram(
	unsigned __int32* histogram,
	unsigned __int8* pixel_buffer,
	unsigned __int32 number_of_pixels) {

	// empty image buffer
	if (number_of_pixels < 1) { return false; }

	// zero-initialize histogram
	memset(histogram, 0u, 256 * sizeof(unsigned __int32));

	unsigned __int32 i{};

	for (i = 0u; i < number_of_pixels; ++i) { ++histogram[pixel_buffer[i]]; }

	return true;
}
