#pragma once
#include "RuntimeDll.hpp"
#include <QLibrary>

namespace runtime_dll {

	bool load_dlls() {
		QLibrary asm_dll{asm_library_name};
		QLibrary cpp_dll{cpp_library_name};

		asm_dll.load();
		cpp_dll.load();

		if (cpp_dll.isLoaded() && asm_dll.isLoaded()) {
			_asm_calculate_histogram =
				reinterpret_cast<calculate_histogram_function_t>(
					asm_dll.resolve(asm_function_name));

			_cpp_calculate_histogram =
				reinterpret_cast<calculate_histogram_function_t>(
					cpp_dll.resolve(cpp_function_name));
		}

		return ((_cpp_calculate_histogram != nullptr) &&
			(_asm_calculate_histogram != nullptr));
	}

}
