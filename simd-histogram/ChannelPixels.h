#pragma once
#include <unordered_map>
#include <optional>

#include "Channel.h"

class ChannelPixels {
public:
	ChannelPixels();
	ChannelPixels(ChannelPixels &&) = default;
	ChannelPixels& operator=(ChannelPixels&&) = default;
	ChannelPixels& operator=(const ChannelPixels&) = delete;
	ChannelPixels(const ChannelPixels &) = delete;
	~ChannelPixels();
	void load_image();
	std::optional<std::unordered_map<channel, unsigned __int8 *>> channels() const;
	const unsigned __int32 number_of_pixels() const noexcept;

private:
	static constexpr std::size_t Alignment = 32;
	std::unordered_map<channel, unsigned __int8 *> _channels;
	unsigned __int32 _number_of_pixels;
};
