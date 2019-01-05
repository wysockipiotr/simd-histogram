#pragma once

#include <QtWidgets/QMainWindow>
#include "Utils/Shared.hpp"
#include "Processing/ProcessingService.hpp"

class MainWindow : public QMainWindow {
Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = Q_NULLPTR);

private:
	void _init_widgets();
	void _connect_signals();
	void _load_image_pressed();

	class ControlWidget* m_control_widget{};
	class HistogramsWidget* m_histograms_widget{};
	class ImageLabel* m_image_preview_widget{};
	class LogWidget* m_logger_widget{};
	QDockWidget* m_image_preview_dock{};

	ProcessingService m_processing_service;

	calculation_policy m_calculation_policy{calculation_policy::cpp};
};
