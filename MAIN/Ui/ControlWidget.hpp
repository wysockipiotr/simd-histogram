#pragma once
#include <QWidget>
#include "Utils/Shared.hpp"

class ControlWidget : public QWidget {
Q_OBJECT
public:
	ControlWidget(QWidget * parent = nullptr);

public slots:
	void lock_load_image_button(bool lock);

	void lock_generate_histograms_button(bool lock);

	void lock_run_benchmark_from_image_button(bool lock);

	void lock_tasks_number_spinbox(bool lock);

	void lock_all_controls(bool lock);

signals :
	// params modified
	void calculation_policy_changed(calculation_policy policy);

	void benchmark_iterations_number_changed(int number_of_iterations);

	void number_of_tasks_changed(int number_of_tasks);

	// buttons pressed
	void load_image_pressed();

	void calculate_histogram_pressed();

	void run_benchmark_from_image_pressed();

private:
	void _init_layout();

	void _init_widgets();

	void _connect_signals();

	// buttons
	class QPushButton * load_image_btn {};
	class QPushButton * generate_histogram_btn {};
	class QPushButton * m_benchmark_from_image_btn {};
	class QGroupBox * m_strategy_box {};
	class QRadioButton * m_cpp_strategy_radio {};

	// spinboxes
	class QSpinBox * m_iterations_number_spinbox {};
	class QSpinBox * m_number_of_tasks_spinbox {};
};
