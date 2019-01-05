#pragma once
#include <QWidget>
#include "Utils/Shared.hpp"

class QPushButton;
class QLabel;

class ControlWidget : public QWidget {
Q_OBJECT
public:
	ControlWidget(QWidget* parent = nullptr);

public slots:
	void lock_load_image_button(bool lock);
	void lock_generate_histograms_button(bool lock);

signals:
	void calculation_policy_changed(calculation_policy policy);
	void load_image_pressed();
	void calculate_histogram_pressed();

private:
	void init_layout();
	void init_widgets();
	void connect_signals();

	QPushButton* load_image_btn{};
	QPushButton* generate_histogram_btn{};
	QPushButton* export_histogram_btn{};
	QPushButton* run_benchmark_btn{};

	class QGroupBox* m_benchmark_group{};
};
