#include <Shared.h>

namespace dll_histogram {
using function = bool (*)(histogram_entry_t* , pixel_t*, unsigned __int32);

	inline function _asm_calculate_histogram = nullptr;
	inline function _cpp_calculate_histogram = nullptr;

	const auto asm_library_name{"DLLASM"};
	const auto asm_function_name{"_asm_calculate_histogram"};
	const auto cpp_library_name{"DLLCPP"};
	const auto cpp_function_name{"_cpp_calculate_histogram"};
}
