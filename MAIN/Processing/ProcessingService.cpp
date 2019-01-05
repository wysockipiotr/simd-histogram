#pragma once

#include <execution>

#include "ProcessingService.hpp"

#include <QImage>
#include <QRgb>
#include <QtConcurrent/QtConcurrentRun>
#include <optional>
#include "Utils/RuntimeDll.hpp"
#include "Utils/Shared.hpp"

ProcessingService::ProcessingService(QObject* parent) : QObject(parent) {
	connect(&this->channels_loaded_watcher,
	        &QFutureWatcher<QFutureWatcher<pixel_buffer_bundle_t>>::finished,
	        [this] {
		        this->channels = this->channels_loaded_watcher.result();
		        emit channels_ready();
	        });
	connect(&this->histograms_calculated_watcher,
	        &QFutureWatcher<std::optional<timed_result<histogram_bundle_t>>>::finished,
	        [this] {
		        const auto [histograms, elapsed] = *(histograms_calculated_watcher.result());
		        this->histograms = histograms;
		        m_channels_are_loaded = true;
		        emit histograms_calculated(elapsed, histograms);
	        });
}

void ProcessingService::load_channels(const QImage& image) {
	const auto load_channels_from_img{
		[=] {
			const auto number_of_pixels{image.width() * image.height()};

			// create and resize pixel buffer for every channel
			pixel_buffer_bundle_t channel_buffers{};
			for (auto& buffer : channel_buffers) { buffer.reserve(number_of_pixels); }

			const QRgb* img_pixels =
				static_cast<const QRgb*>(static_cast<const void*>(image.bits()));

			const auto to_pixel_buffer_bundle{
				[&channel_buffers](const QRgb& pixel) {
					channel_buffers[channel::red].push_back(
						static_cast<unsigned __int8>(qRed(pixel)));
					channel_buffers[channel::green].push_back(
						static_cast<unsigned __int8>(qGreen(pixel)));
					channel_buffers[channel::blue].push_back(
						static_cast<unsigned __int8>(qBlue(pixel)));
					channel_buffers[channel::luminance].push_back(
						static_cast<unsigned __int8>(qGray(pixel)));
				}
			};

			// split QRgb data into separate channels of a format suitable for further computation
			std::for_each(
				std::execution::par_unseq,
				img_pixels,
				img_pixels + number_of_pixels,
				to_pixel_buffer_bundle
			);

			return channel_buffers;
		}
	};
	m_channels_are_loaded = false;
	const auto fetch = QtConcurrent::run(load_channels_from_img);
	channels_loaded_watcher.setFuture(fetch);
}

void ProcessingService::calculate_histograms(calculation_policy policy) {

	using namespace std::chrono_literals;

	const auto calc_histogram_function{
		(policy == calculation_policy::cpp)
			? runtime_dll::_cpp_calculate_histogram
			: runtime_dll::_asm_calculate_histogram
	};

	const auto calculate_all{
		[=]() -> std::optional<timed_result<histogram_bundle_t>> {
			histogram_bundle_t histograms{};
			histogram_t histogram(256, 0);

			std::chrono::duration<double> total_duration{0s};
			bool result{};

			for (channel::index i = 0; i < channel::end; ++i) {
				const auto start{std::chrono::high_resolution_clock::now()};
				result = calc_histogram_function(
					histogram.data(), this->channels[i].data(),
					this->channels[i].size());
				const auto stop{std::chrono::high_resolution_clock::now()};
				total_duration += (stop - start);
				if (!result) { return std::nullopt; }
				histograms[i] = histogram;
			}


			return {{histograms, total_duration}};
		}
	};

	const auto future{QtConcurrent::run(calculate_all)};
	histograms_calculated_watcher.setFuture(future);
}

void ProcessingService::run_benchmark() { }
