#pragma once
#include <memory>
#include "CalculationStrategy.h"


class ChannelPixels;

class Histogram {
public:
	void use_asm(bool use);

	bool calculate(const ChannelPixels & pixels);

private:
	std::unique_ptr<CalculationStrategy> strategy{ std::make_unique<CppCalculationStrategy>() };

	__declspec(align(16)) unsigned __int32 red[256]{};
	__declspec(align(16)) unsigned __int32 green[256]{};
	__declspec(align(16)) unsigned __int32 blue[256]{};
	__declspec(align(16)) unsigned __int32 lum[256]{};
};
