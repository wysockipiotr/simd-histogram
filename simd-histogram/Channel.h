#pragma once

enum class channel : std::size_t { r, g, b, lum };

static const channel all_channels[] = { channel::r, channel::g, channel::b, channel::lum };