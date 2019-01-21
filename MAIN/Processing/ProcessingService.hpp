#pragma once
#include <QFutureWatcher>
#include <QObject>
#include <optional>
#include "HistogramsWidget.hpp"

enum class calculation_policy;

class ProcessingService : public QObject {
Q_OBJECT
public:
	explicit ProcessingService(QObject * parent = nullptr);

	bool channels_are_loaded() const {
		return m_channels_are_loaded;
	}

public slots:
	void load_channels(const QImage & image);

	void calculate_histograms(calculation_policy policy, unsigned number_of_tasks);

	void run_benchmark(unsigned number_of_tasks,
	                   unsigned number_of_iterations);

signals:
	void channels_ready();

	void histograms_calculated(std::chrono::duration<double> elapsed, const histogram_bundle_t & histograms);

	void benchmark_complete(benchmark_result_t result);

private:

	void connect_signals();

	bool m_channels_are_loaded { false };

	pixel_buffer_bundle_t channels;

	std::optional<histogram_bundle_t> histograms;

	QFutureWatcher<pixel_buffer_bundle_t> channels_loaded_watcher;

	QFutureWatcher<std::optional<timed_result<histogram_bundle_t>>> histograms_calculated_watcher;

	QFutureWatcher<std::optional<benchmark_result_t>> m_benchmark_complete_watcher;
};
