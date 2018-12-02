#pragma once

extern "C" bool AsmCalculateHistogram(unsigned __int32 number_of_pixels, unsigned __int8 * pixels, unsigned __int32 * histogram);

struct CalculationStrategy {
	virtual bool calculate(unsigned __int32 number_of_pixels, unsigned __int8 * pixels, unsigned __int32 * histogram) = 0;
	virtual ~CalculationStrategy() = default;
};

struct CppCalculationStrategy : public CalculationStrategy {
	bool calculate(unsigned __int32 number_of_pixels, unsigned __int8 *pixels, unsigned __int32 *histogram) override {
		for (unsigned __int32 i = 0u; i < number_of_pixels; ++i) {
			++histogram[pixels[i]];
		}
		return true;
	}
};

struct AsmCalculationStrategy : public CalculationStrategy {
	bool calculate(unsigned __int32 number_of_pixels, unsigned __int8 *pixels, unsigned __int32 *histogram) override {
		return AsmCalculateHistogram(number_of_pixels, pixels, histogram);
	}
};