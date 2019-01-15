#include <QtConcurrent/QtConcurrentRun>

#include <algorithm>
#include <numeric>
#include <future>
#include <optional>
#include <execution>

#include "ProcessingService.hpp"
#include "Utils/RuntimeDll.hpp"
#include "Utils/Shared.hpp"

namespace {
	/**
	 * Returns sum of two intermediate histograms
	 */
	auto histogram_add(const histogram_t & lhs, const histogram_t & rhs) {
		histogram_t result(256, 0);
		std::transform(
			std::execution::par_unseq,
			lhs.cbegin(),
			lhs.cend(),
			rhs.cbegin(),
			result.begin(),
			std::plus<> {});
		return result;
	}


	/**
	 * Returns nearest multiple of 16 lower than or equal to x  
	 */
	auto prev_multiple_of_16(unsigned __int32 x) {
		return ( x & ~0xf );
	};


	/**
	 * Calculates histogram in parallel (if number of pixels > 1024, due to alignment requirements).
	 * 
	 * @param number_of_tasks Number of calculation tasks
	 * @param calculate_histogram Histogram calculating function (either c++ or assembly)
	 * @param pixels Channel pixel buffer
	 * 
	 * @return Histogram or absent value if an error occured
	 */
	std::optional<histogram_t> parallel_calc_histogram_(unsigned number_of_tasks,
	                                                    runtime_dll::calculate_histogram_function_t calculate_histogram,
	                                                    const pixel_buffer_t & pixels) {

		const auto number_of_pixels { pixels.size() };

		// non-parallel execution if number of pixels is less than 1024 (64 (max num of threads) * 16, 16 is required alignment for a batch)
		if (number_of_pixels < 1024) {
			histogram_t histogram(256, 0);
			return calculate_histogram(
				       histogram.data(),
				       pixels.data(),
				       number_of_pixels)
				       ? std::make_optional(histogram)
				       : std::nullopt;
		}

		// create a separate histogram for each pixels batch
		std::vector<histogram_t> batch_histograms(number_of_tasks, histogram_t(256, 0u));

		const auto batch_size { prev_multiple_of_16(number_of_pixels / number_of_tasks) };
		const auto pixels_ptr { pixels.data() };

		// all pixels range iterator pair 
		const auto begin { pixels_ptr };
		const auto end { pixels_ptr + number_of_pixels };

		// batch pixels range iterator pair
		auto from { begin };
		auto to { begin };

		// future batch calculations results
		std::vector<std::future<bool>> tasks;
		tasks.reserve(number_of_tasks);

		// split pixels between tasks and run these tasks concurrently
		for (unsigned i { 0 }; i < number_of_tasks; ++i) {
			from = to;
			if (i == number_of_tasks - 1) {
				to = end;
			} else {
				std::advance(to, batch_size);
			}

			tasks.emplace_back(std::async(
					std::launch::async,
					calculate_histogram,
					batch_histograms[i].data(),
					from,
					static_cast<unsigned __int32>(std::distance(from, to)))
			);
		}

		// obtain correctness of batch calculations
		std::vector<bool> results;
		results.reserve(number_of_tasks);
		for (auto & t : tasks) {
			results.push_back(t.get());
		}
		const bool calculations_are_correct {
			std::accumulate(std::cbegin(results), std::cend(results), true, std::bit_and {})
		};

		// return intermediate histograms reduced to one resultant or absent value if an error occured
		return ( calculations_are_correct
			         ? std::make_optional(std::reduce(
				         std::execution::par_unseq,
				         batch_histograms.cbegin(),
				         batch_histograms.cend(),
				         histogram_t(256, 0),
				         histogram_add))
			         : std::nullopt );
	}


	/**
	 * Calculate all four histograms of loaded pixel buffers using specified calculation policy and number of tasks.
	 * 
	 * @param policy Calculation policy (C++ or assembly)
	 * @param channels Bundle of pixel buffers for all channels
	 * @param number_of_tasks Number of tasks
	 * 
	 * @return Bundle of all four histograms with total execution time, or absent value if an error occured
	 */
	std::optional<timed_result<histogram_bundle_t>> calculate_all_(calculation_policy policy,
	                                                               pixel_buffer_bundle_t channels,
	                                                               unsigned number_of_tasks) {

		// set function corresponding to the selected policy
		const auto calc_histogram_function {
			( policy == calculation_policy::cpp )
				? runtime_dll::_cpp_calculate_histogram
				: runtime_dll::_asm_calculate_histogram
		};

		histogram_bundle_t histograms {};

		std::chrono::duration<double> total_duration { 0 };

		std::optional<histogram_t> result {};

		for (channel::index i = 0; i < channel::end; ++i) {
			const auto start { std::chrono::high_resolution_clock::now() };
			result = parallel_calc_histogram_(number_of_tasks, calc_histogram_function, channels[i]);
			const auto stop { std::chrono::high_resolution_clock::now() };

			total_duration += ( stop - start );

			if (!result.has_value()) {
				return std::nullopt;
			}

			histograms[i] = *result;
		}

		return { { histograms, total_duration } };
	}
} // namespace


ProcessingService::ProcessingService(QObject * parent)
	: QObject(parent) {
	connect(&this->channels_loaded_watcher,
	        &QFutureWatcher<QFutureWatcher<pixel_buffer_bundle_t>>::finished,
	        [this] {
		        this->channels = this->channels_loaded_watcher.result();
		        emit channels_ready();
	        });

	connect(&this->histograms_calculated_watcher,
	        &QFutureWatcher<std::optional<timed_result<histogram_bundle_t>>>::finished,
	        [this] {
		        if (histograms_calculated_watcher.result().has_value()) {
			        const auto [histograms, elapsed] =
					        *( histograms_calculated_watcher.result() );
			        this->histograms = histograms;
			        m_channels_are_loaded = true;
			        emit histograms_calculated(elapsed, histograms);
		        }
	        });

	connect(&m_benchmark_complete_watcher, &QFutureWatcher<std::optional<benchmark_result_t>>::finished, [this] {
		const auto result { m_benchmark_complete_watcher.result() };
		if (result.has_value()) {
			emit benchmark_complete(*result);
		}
	});
}

void ProcessingService::load_channels(const QImage & image) {
	const auto load_channels_from_img {
		[=] {
			const auto number_of_pixels { image.width() * image.height() };

			// create and resize pixel buffer for every channel
			pixel_buffer_bundle_t channel_buffers {};
			for (auto & buffer : channel_buffers) {
				buffer.reserve(number_of_pixels);
			}

			const auto img_pixels { static_cast<const QRgb*>(static_cast<const void*>(image.bits())) };

			const auto to_pixel_buffer_bundle {
				[&channel_buffers](const QRgb & pixel) {
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

void ProcessingService::calculate_histograms(calculation_policy policy,
                                             unsigned number_of_tasks) {
	const auto future { QtConcurrent::run(calculate_all_, policy, this->channels, number_of_tasks) };
	histograms_calculated_watcher.setFuture(future);
}

void ProcessingService::run_benchmark(unsigned number_of_tasks,
                                      unsigned number_of_iterations) {

	const auto calculate_average_elapsed {
		[this, number_of_iterations, number_of_tasks](
	calculation_policy policy) -> std::optional<std::chrono::duration<double>> {

			std::vector<std::chrono::duration<double>> elapsed_values(number_of_iterations);

			for (std::size_t i { 0 }; i < number_of_iterations; ++i) {
				const auto result {
					calculate_all_(policy, this->channels, number_of_tasks)
				};

				if (result.has_value()) {
					const auto [histogram, elapsed] = *result;
					elapsed_values[i] = elapsed;
				} else {
					return std::nullopt;
				}
			}
			return std::make_optional(
				std::accumulate(elapsed_values.cbegin(),
				                elapsed_values.cend(),
				                std::chrono::duration<double> {}) / number_of_iterations
			);
		}
	};

	const auto run_benchmark_job {
		[calculate_average_elapsed]() -> std::optional<benchmark_result_t> {
			const auto cpp { calculate_average_elapsed(calculation_policy::cpp) };
			const auto assembly { calculate_average_elapsed(calculation_policy::assembly) };

			if (cpp.has_value() && assembly.has_value()) {
				const benchmark_result_t result { *cpp, *assembly };
				return result;
			}
			return std::nullopt;
		}
	};

	const auto future_benchmark_results { QtConcurrent::run(run_benchmark_job) };
	m_benchmark_complete_watcher.setFuture(future_benchmark_results);
}
