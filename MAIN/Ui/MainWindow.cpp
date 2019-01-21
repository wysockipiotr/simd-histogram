#include "MainWindow.hpp"
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <thread>
#include "Ui/ControlWidget.hpp"
#include "Ui/HistogramsWidget.hpp"
#include "Ui/ImagePreviewWidget.hpp"
#include "Ui/LogWidget.hpp"

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent),
	  m_number_of_threads(std::thread::hardware_concurrency()) {

	setWindowTitle(tr("Histogram calculator"));

	// fix initial value of threads if hardware_concurrency value is unavailable
	if (m_number_of_threads == 0) {
		m_number_of_threads = 1;
	}

	// set initial window size
	this->resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

	_init_widgets();
	_connect_signals();
}

void MainWindow::_load_image_pressed() {
	m_control_widget->lock_load_image_button(true);
	m_control_widget->lock_generate_histograms_button(true);

	QString filename = QFileDialog::getOpenFileName(
		this, "Load image", QDir::homePath(),
		tr("Image files (*.png *.jpg *.jpeg *.bmp)"));

	// filename is empty if user dismissed file dialog
	if (!filename.isEmpty()) {
		QImage image { filename };
		image = image.convertToFormat(QImage::Format_RGB32);
		m_control_widget->lock_load_image_button(false);

		if (!image.isNull()) {
			m_control_widget->lock_load_image_button(true);

			const QString loaded_image_info_message {
				tr("Loading image \"%1\" (%2x%3) Total %4 pixels")
				.arg(filename)
				.arg(image.width())
				.arg(image.height())
				.arg(image.width() * image.height())
			};

			m_logger_widget->print_message(loaded_image_info_message);

			m_processing_service.load_channels(image);
			m_image_preview_widget->set_image(image);

		} else {
			QMessageBox msg_box {
				QMessageBox::NoIcon, QString { "Error" },
				QString { "File unsupported." }, QMessageBox::Ok, this
			};
			msg_box.exec();
		}
	} else {
		m_control_widget->lock_load_image_button(false);
		if (m_processing_service.channels_are_loaded()) {
			m_control_widget->lock_generate_histograms_button(false);
		}
	}
}

void MainWindow::_calculate_histogram_pressed() {
	m_histograms_widget->clear();
	// m_control_widget->lock_generate_histograms_button(true);
	// m_control_widget->lock_tasks_number_spinbox(true);
	m_control_widget->lock_all_controls(true);
	m_processing_service.calculate_histograms(m_calculation_policy,
	                                          m_number_of_threads);
}

void MainWindow::_run_benchmark_pressed() {
	m_control_widget->lock_all_controls(true);
	m_processing_service.run_benchmark(m_number_of_threads,
	                                   m_benchmark_number_of_iterations);
	m_logger_widget->print_message(
		tr("Running benchmark (number of iterations: %1)")
		.arg(m_benchmark_number_of_iterations));
}

void MainWindow::_channels_ready() const {
	m_logger_widget->print_message(tr("Image loaded successfully"));
	m_control_widget->lock_load_image_button(false);
	m_control_widget->lock_generate_histograms_button(false);
	m_control_widget->lock_run_benchmark_from_image_button(false);
	m_image_preview_dock->setVisible(true);
}

void MainWindow::_histograms_calculated(duration_t duration,
                                        histogram_bundle_t histograms) const {
	const auto message {
		tr("Histograms calculated successfully (number of "
			"threads: %1, elapsed: %2 ms)")
		.arg(m_number_of_threads)
		.arg(milliseconds(duration).count())
	};
	m_logger_widget->print_message(message);

	m_histograms_widget->plot(histograms);
	// m_control_widget->lock_generate_histograms_button(false);
	// m_control_widget->lock_load_image_button(false);
	// m_control_widget->lock_tasks_number_spinbox(false);

	m_control_widget->lock_all_controls(false);
}

void MainWindow::_calculation_policy_changed(calculation_policy policy) {
	this->m_calculation_policy = policy;

	this->m_logger_widget->print_message(
		tr("Strategy changed to %1")
		.arg(policy == calculation_policy::cpp ? "C++" : "assembly"));
}

void MainWindow::_benchmark_complete(benchmark_result_t result) {
	m_logger_widget->print_message(
		tr("Benchmark result:<br/>C++: %1 ms<br/>Assembly: %2 ms")
		.arg(milliseconds(result.cpp_elapsed).count())
		.arg(milliseconds(result.asm_elapsed).count()));
	m_control_widget->lock_all_controls(false);
}

void MainWindow::_benchmark_iterations_number_changed(
	unsigned number_of_iterations) {
	m_benchmark_number_of_iterations = number_of_iterations;
}

void MainWindow::_number_of_threads_changed(unsigned number_of_threads) {
	m_number_of_threads = number_of_threads;
	m_logger_widget->print_message(
		tr("Number of threads changed to %1").arg(number_of_threads));
}

void MainWindow::_init_widgets() {
	// histograms widget (central)
	m_histograms_widget = new HistogramsWidget{ this };
	setCentralWidget(m_histograms_widget);

	// image preview dock widget
	m_image_preview_widget = new ImagePreviewWidget{ this };
	m_image_preview_dock = new QDockWidget{ tr("Image preview"), this };
	m_image_preview_dock->setWidget(m_image_preview_widget);
	m_image_preview_dock->setAllowedAreas(Qt::RightDockWidgetArea);
	m_image_preview_dock->setFeatures(QDockWidget::DockWidgetMovable);
	m_image_preview_dock->setVisible(false);
	addDockWidget(Qt::RightDockWidgetArea, m_image_preview_dock);

	// control widget
	m_control_widget = new ControlWidget{ this };
	auto control_dock { new QDockWidget{ tr("Control panel"), this } };
	control_dock->setWidget(m_control_widget);
	control_dock->setFeatures(QDockWidget::DockWidgetMovable);
	control_dock->setAllowedAreas(Qt::RightDockWidgetArea |
	                              Qt::LeftDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, control_dock);

	// logger widget
	m_logger_widget = new LogWidget{ this };
	auto logger_dock { new QDockWidget{ tr("Log"), this } };
	logger_dock->setWidget(m_logger_widget);
	logger_dock->setFeatures(QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::RightDockWidgetArea, logger_dock);
}

void MainWindow::_connect_signals() const {
	connect(m_control_widget, &ControlWidget::load_image_pressed, this,
	        &MainWindow::_load_image_pressed);

	connect(m_control_widget, &ControlWidget::calculate_histogram_pressed, this,
	        &MainWindow::_calculate_histogram_pressed);

	connect(m_control_widget, &ControlWidget::calculation_policy_changed, this,
	        &MainWindow::_calculation_policy_changed);

	connect(&m_processing_service, &ProcessingService::channels_ready, this,
	        &MainWindow::_channels_ready);

	connect(&m_processing_service, &ProcessingService::histograms_calculated,
	        this, &MainWindow::_histograms_calculated);

	connect(&m_processing_service, &ProcessingService::benchmark_complete, this,
	        &MainWindow::_benchmark_complete);

	connect(m_control_widget, &ControlWidget::benchmark_iterations_number_changed,
	        this, &MainWindow::_benchmark_iterations_number_changed);

	connect(m_control_widget, &ControlWidget::number_of_tasks_changed, this,
	        &MainWindow::_number_of_threads_changed);

	connect(m_control_widget, &ControlWidget::run_benchmark_from_image_pressed,
	        this, &MainWindow::_run_benchmark_pressed);
}
