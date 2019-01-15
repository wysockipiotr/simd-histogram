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
	void init_layout();

	void init_widgets();

	void connect_signals();

	// buttons
	class QPushButton * load_image_btn {};
	class QPushButton * generate_histogram_btn {};
	class QPushButton * m_benchmark_from_image_btn {};
	class QPushButton * m_benchmark_from_generated_btn {};

	// spinboxes
	class QSpinBox * m_iterations_number_spinbox {};
	class QSpinBox * m_number_of_tasks_spinbox {};
};
