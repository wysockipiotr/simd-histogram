#pragma once

#include <QtWidgets/QMainWindow>
#include "Utils/Shared.hpp"
#include "Processing/ProcessingService.hpp"

class MainWindow : public QMainWindow {
Q_OBJECT
public:
	explicit MainWindow(QWidget * parent = Q_NULLPTR);

private slots:

	void _load_image_pressed();

	void _calculate_histogram_pressed();

	void _run_benchmark_pressed();

	void _channels_ready() const;

	void _histograms_calculated(duration_t duration, histogram_bundle_t histograms) const;

	void _calculation_policy_changed(calculation_policy policy);

	void _benchmark_complete(benchmark_result_t result);

	void _benchmark_iterations_number_changed(unsigned);

	void _number_of_threads_changed(unsigned);

private:
	void _init_widgets();

	void _connect_signals() const;

	// widgets
	class ControlWidget * m_control_widget {};
	class HistogramsWidget * m_histograms_widget {};
    class ImagePreviewWidget* m_image_preview_widget{};
	class LogWidget * m_logger_widget {};
	class QDockWidget * m_image_preview_dock {};

	// services
	ProcessingService m_processing_service;

	// calculations parameters
	calculation_policy m_calculation_policy { calculation_policy::cpp };
	unsigned m_number_of_threads {};
	unsigned m_benchmark_number_of_iterations { 100 };
};
