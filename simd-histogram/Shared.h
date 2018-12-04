#pragma once
#include <vector>
#include "AlignedAllocator.h"

using histogram_t = std::vector<unsigned __int32, aligned_allocator<unsigned __int32, 16>>;
using pixel_buffer_t = std::vector<unsigned __int8, aligned_allocator<unsigned __int8, 32>>;

using histograms_t = std::array<histogram_t, 4>;

namespace channel {
	const std::size_t red{ 0 };
	const std::size_t green{ 1 };
	const std::size_t blue{ 2 };
	const std::size_t luminance{ 3 };
}

enum class calculation_policy { cpp, assembly };