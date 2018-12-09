#include "Utils/Shared.hpp"

namespace dll_histogram {
	using function = bool (*)(histogram_entry_t* , const pixel_t*, unsigned __int32);

	inline function _asm_calculate_histogram = nullptr;
	inline function _cpp_calculate_histogram = nullptr;

	static const auto asm_library_name{"DLLASM"};
	static const auto asm_function_name{"_asm_calculate_histogram"};
				 
	static const auto cpp_library_name{"DLLCPP"};
	static const auto cpp_function_name{"_cpp_calculate_histogram"};
}
