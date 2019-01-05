#include "ControlWidget.hpp"
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGroupBox>

ControlWidget::ControlWidget(QWidget* parent) {
	init_widgets();
	init_layout();
	connect_signals();
}

void ControlWidget::lock_load_image_button(bool lock) { this->load_image_btn->setDisabled(lock); }

void ControlWidget::lock_generate_histograms_button(bool lock) { this->generate_histogram_btn->setDisabled(lock); }

void ControlWidget::init_layout() {
	// init layouts
	auto column{new QVBoxLayout{this}};

	// strategy radio group box
	auto strategy_box{new QGroupBox{"Histogram calculation policy", this}};
	auto cpp_strategy_radio{new QRadioButton{"C++", strategy_box}};
	auto asm_strategy_radio{new QRadioButton{"Assembly", strategy_box}};
	auto radio_box_layout{new QHBoxLayout{this}};

	radio_box_layout->addWidget(cpp_strategy_radio);
	radio_box_layout->addWidget(asm_strategy_radio);
	strategy_box->setLayout(radio_box_layout);
	// strategy_box->setDisabled(true);
	cpp_strategy_radio->setChecked(true);

	// buttons layout
	auto buttons_layout{new QHBoxLayout{this}};
	buttons_layout->addWidget(this->load_image_btn);
	buttons_layout->addWidget(this->generate_histogram_btn);

	// insert control widgets into column
	column->addLayout(buttons_layout);
	column->addWidget(strategy_box);
	column->addSpacing(24);
	column->addStretch(1);
	column->addWidget(this->export_histogram_btn);
	column->addWidget(this->run_benchmark_btn);

	column->setMargin(16);

	connect(cpp_strategy_radio, &QRadioButton::toggled,
	        [=] {
		        emit calculation_policy_changed(
			        (cpp_strategy_radio->isChecked())
				        ? calculation_policy::cpp
				        : calculation_policy::assembly);
	        });
}

void ControlWidget::init_widgets() {
	// buttons
	this->load_image_btn = new QPushButton{"Load image", this};
	this->load_image_btn->setDefault(true);
	this->generate_histogram_btn = new QPushButton{"Calculate histogram", this};
	this->generate_histogram_btn->setEnabled(false);
	this->export_histogram_btn = new QPushButton{"Export histogram (*.csv)"};
	this->export_histogram_btn->setEnabled(false);
	this->run_benchmark_btn = new QPushButton{"Run benchmark", this};

	m_benchmark_group = new QGroupBox{tr("Benchmark settings"), this};
}

void ControlWidget::connect_signals() {
	connect(this->load_image_btn, SIGNAL(pressed()), this, SIGNAL(load_image_pressed()));
	connect(this->generate_histogram_btn, SIGNAL(pressed()), this, SIGNAL(calculate_histogram_pressed()));
}
