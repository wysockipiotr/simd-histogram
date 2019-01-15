#pragma once
#include "Cpuinfo.hpp"

#include <array>
#include <bitset>
#include <intrin.h> // __cpuid, __cpuidex

bool cpuinfo::avx_is_supported() {
	constexpr static auto k_avx_flag{28};
	std::array<int, 4> cpuid {};

	__cpuid(cpuid.data(), 0x0);
	const auto highest_valid_id{cpuid.at(0)};

	if (highest_valid_id > 0) {
		__cpuidex(cpuid.data(), 1, 0x0);
		std::bitset<32> row = cpuid.at(2);
		return row[k_avx_flag];
	}
	return false;
}
