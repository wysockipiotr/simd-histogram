#include "ControlWidget.hpp"

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include <thread>

ControlWidget::ControlWidget(QWidget * parent) {
	init_widgets();
	init_layout();
	connect_signals();
}

void ControlWidget::lock_load_image_button(bool lock) {
	this->load_image_btn->setDisabled(lock);
}

void ControlWidget::lock_generate_histograms_button(bool lock) {
	this->generate_histogram_btn->setDisabled(lock);
}

void ControlWidget::lock_run_benchmark_from_image_button(bool lock) {
  m_benchmark_from_image_btn->setDisabled(lock);
}

void ControlWidget::init_layout() {

	// init layouts
	auto column { new QVBoxLayout{ this } };

	// strategy radio group box
	auto strategy_box { new QGroupBox{ tr("Histogram calculation policy"), this } };
	const auto cpp_strategy_radio { new QRadioButton{ tr("C++"), strategy_box } };
	const auto asm_strategy_radio { new QRadioButton{ tr("Assembly"), strategy_box } };
	const auto radio_box_layout { new QHBoxLayout{ this } };

	radio_box_layout->addWidget(cpp_strategy_radio);
	radio_box_layout->addWidget(asm_strategy_radio);
	strategy_box->setLayout(radio_box_layout);
	cpp_strategy_radio->setChecked(true);

	// buttons layout
	auto buttons_layout { new QHBoxLayout{ this } };
	buttons_layout->addWidget(this->load_image_btn);
	buttons_layout->addWidget(this->generate_histogram_btn);

	// insert control widgets into column
	column->addLayout(buttons_layout);
	column->addWidget(strategy_box);

	// task / thread number control
	auto task_number_layout { new QHBoxLayout{ this } };
	auto task_number_label { new QLabel{ tr("Concurrency level"), this } };
	task_number_layout->addWidget(task_number_label);
	task_number_layout->addWidget(m_number_of_tasks_spinbox);
	column->addLayout(task_number_layout);

	column->addSpacing(24);
	column->addStretch(1);

	// benchmark layout
	column->addWidget(new QLabel{ tr("Benchmark histogram functions"), this });
	auto iterations_spinbox_layout { new QHBoxLayout{ this } };
	auto iterations_label { new QLabel{ tr("Iterations"), this } };
	iterations_spinbox_layout->addWidget(iterations_label);
	iterations_spinbox_layout->addWidget(m_iterations_number_spinbox);
	column->addLayout(iterations_spinbox_layout);
	column->addWidget(m_benchmark_from_image_btn);
	column->addWidget(m_benchmark_from_generated_btn);

	column->setMargin(16);

	connect(cpp_strategy_radio, &QRadioButton::toggled, [=] {
		emit calculation_policy_changed(( cpp_strategy_radio->isChecked() )
			                                ? calculation_policy::cpp
			                                : calculation_policy::assembly);
	});
}

void ControlWidget::init_widgets() {
	// buttons
	this->load_image_btn = new QPushButton{ "Load image", this };
	this->load_image_btn->setDefault(true);
	this->generate_histogram_btn = new QPushButton{ "Calculate histogram", this };
	this->generate_histogram_btn->setEnabled(false);

	m_benchmark_from_image_btn = new QPushButton{ tr("Benchmark (use image)"), this };
	m_benchmark_from_image_btn->setDisabled(true);
	m_benchmark_from_generated_btn = new QPushButton{ tr("Benchmark (generate random)"), this };

	// spinboxes
	const char * spinbox_stylesheet { "QSpinBox { padding: 5px; }" };

	m_iterations_number_spinbox = new QSpinBox{ this };
	m_iterations_number_spinbox->setRange(1, 1000);
	m_iterations_number_spinbox->setSingleStep(10);
	m_iterations_number_spinbox->setValue(100);
	m_iterations_number_spinbox->setStyleSheet(spinbox_stylesheet);

	auto default_tasks_number { std::thread::hardware_concurrency() };
	if (default_tasks_number == 0) {
		default_tasks_number = 1;
	}

	m_number_of_tasks_spinbox = new QSpinBox{ this };
	m_number_of_tasks_spinbox->setRange(1, 64);
	m_number_of_tasks_spinbox->setSingleStep(1);
	m_number_of_tasks_spinbox->setValue(default_tasks_number);
	m_number_of_tasks_spinbox->setStyleSheet(spinbox_stylesheet);
}

void ControlWidget::connect_signals() {

	// button presses
	connect(this->load_image_btn,
	        SIGNAL(pressed()),
	        this,
	        SIGNAL(load_image_pressed()));

	connect(this->generate_histogram_btn,
	        SIGNAL(pressed()),
	        this,
	        SIGNAL(calculate_histogram_pressed()));

	connect(m_benchmark_from_image_btn,
	        SIGNAL(pressed()),
	        this,
	        SIGNAL(run_benchmark_from_image_pressed()));

	// spinbox value changes
	connect(m_iterations_number_spinbox,
	        SIGNAL(valueChanged(int)),
	        this,
	        SIGNAL(benchmark_iterations_number_changed(int)));

	connect(m_number_of_tasks_spinbox,
	        SIGNAL(valueChanged(int)),
	        this,
	        SIGNAL(number_of_tasks_changed(int)));
}
