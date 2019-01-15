#pragma once

#include <array>
#include <vector>

#include <chrono>
#include "AlignedAllocator.hpp"

namespace custom_allocation {
	const std::size_t alignment_16{16u};
	const std::size_t alignment_32{32u};
	const std::size_t alignment_64{64u};
} // namespace custom_allocation

// channels enumeration for convenience
namespace channel {
	using index = std::size_t;

	const std::size_t red{0};
	const std::size_t green{1};
	const std::size_t blue{2};
	const std::size_t luminance{3};

	const std::size_t end{4};
} // namespace channel

// available computation strategies
enum class calculation_policy { cpp, assembly };

// pixel color intensity value (0-255) type
using pixel_t = unsigned __int8;

// value of a single histogram bucket (each bucket contains number of pixels of
// a corresponding color intensity)
using histogram_entry_t = unsigned __int32;

// aligned (16) vector of entries forming a histogram
using histogram_t = std::vector<histogram_entry_t, aligned_allocator<histogram_entry_t, custom_allocation::alignment_16>>;

// ordered quadruple of histograms (red, green, blue, luminance/gray)
using histogram_bundle_t = std::array<histogram_t, 4>;

// aligned (32) vector of pixels loaded from an image channel
using pixel_buffer_t =
std::vector<pixel_t,
            aligned_allocator<pixel_t, 64>>;

// ordered quadruple of pixel buffers (red, green, blue, luminance/gray)
using pixel_buffer_bundle_t = std::array<pixel_buffer_t, 4>;

template <typename T>
using timed_result = std::pair<T, std::chrono::duration<double>>;

struct benchmark_result_t {
  std::chrono::duration<double> cpp_elapsed;
  std::chrono::duration<double> asm_elapsed;
};

// std time units
using seconds = std::chrono::duration<double>;
using milliseconds =
std::chrono::duration<double,
                      std::ratio_multiply<seconds::period, std::milli>>;
using microseconds =
std::chrono::duration<double,
                      std::ratio_multiply<seconds::period, std::micro>>;
