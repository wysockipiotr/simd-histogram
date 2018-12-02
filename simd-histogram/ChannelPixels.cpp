#include "ChannelPixels.h"

ChannelPixels::ChannelPixels() : _channels(), _number_of_pixels(0) {
		for (auto channel : all_channels) {
			_channels[channel] = nullptr;
		}
}

ChannelPixels::~ChannelPixels() {
	for (auto channel : all_channels) {
		auto pixels = _channels[channel];
		if (pixels) _aligned_free(pixels);
	}
}

void ChannelPixels::load_image() {
	_number_of_pixels = 4096;
	for (auto ch : all_channels) {
		auto pixels = _channels[ch];
		if (pixels) _aligned_free(pixels);
		_channels[ch] = static_cast<unsigned __int8 *>(_aligned_malloc(_number_of_pixels * sizeof(unsigned __int8), Alignment));
	}
}

std::optional<std::unordered_map<channel, unsigned __int8 *>> ChannelPixels::channels() const {
	return _number_of_pixels ? std::make_optional(_channels) : std::nullopt;
}
const unsigned __int32 ChannelPixels::number_of_pixels() const noexcept { return _number_of_pixels; }
