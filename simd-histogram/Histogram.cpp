#include "Histogram.h"
#include <memory>
#include <iostream>
#include "Channel.h"
#include "ChannelPixels.h"


void Histogram::use_asm(bool use) {
		if (use) {
			strategy = std::make_unique<AsmCalculationStrategy>();
		} else {
			strategy = std::make_unique<CppCalculationStrategy>();
		}
	}

	bool Histogram::calculate(const ChannelPixels & pixels) {
		auto channels{ pixels.channels() };
		auto number_of_pixels{ pixels.number_of_pixels() };

		if (channels) {
			bool ok = true;
			ok &= strategy->calculate(number_of_pixels, (*channels)[channel::r], red);
			ok &= strategy->calculate(number_of_pixels, (*channels)[channel::g], green);
			ok &= strategy->calculate(number_of_pixels, (*channels)[channel::b], blue);
			ok &= strategy->calculate(number_of_pixels, (*channels)[channel::lum], lum);

			return ok;
		}

		return false;
	}
