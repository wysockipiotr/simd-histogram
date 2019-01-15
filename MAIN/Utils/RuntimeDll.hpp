#pragma once
#include "Shared.hpp"

namespace runtime_dll {

	using calculate_histogram_function_t =
	bool (*)(histogram_entry_t*, const pixel_t*, unsigned __int32);

	inline calculate_histogram_function_t _asm_calculate_histogram{nullptr};
	inline calculate_histogram_function_t _cpp_calculate_histogram{nullptr};

	static const auto asm_library_name{"DLLASM"};
	static const auto asm_function_name{"_asm_calculate_histogram"};

	static const auto cpp_library_name{"DLLCPP"};
	static const auto cpp_function_name{"_cpp_calculate_histogram"};

	bool load_dlls();

} // namespace dll_histogram
