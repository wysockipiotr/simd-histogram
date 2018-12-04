#include "MainWidget.h"
#include "HistogramsWidget.h"
#include "Shared.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>

#include <execution>
#include <optional>
#include <chrono>

extern "C" unsigned __int32 NUM_PIXELS_MAX = 4294967295;

namespace {
	auto load_channels_from_image(const QImage & image) {
		const auto number_of_pixels{ image.width() * image.height() };
		std::vector<QRgb> pixels(number_of_pixels);
		memmove(pixels.data(), image.bits(), number_of_pixels * sizeof(QRgb)); //TODO: memmove or memcpy?
		std::array<pixel_buffer_t, 4> channel_buffers{};
		for (auto & buffer : channel_buffers) {
			buffer.reserve(number_of_pixels);
		}
		std::for_each(std::execution::par_unseq, std::cbegin(pixels), std::cend(pixels), [&channel_buffers](const QRgb & pixel)
		{
			channel_buffers[channel::red].push_back(static_cast<unsigned __int8>(qRed(pixel)));
			channel_buffers[channel::green].push_back(static_cast<unsigned __int8>(qGreen(pixel)));
			channel_buffers[channel::blue].push_back(static_cast<unsigned __int8>(qBlue(pixel)));
			channel_buffers[channel::luminance].push_back(static_cast<unsigned __int8>(qGray(pixel)));
		});
		return channel_buffers; //std::make_optional(channel_buffers);
	}

	std::optional<histogram_t> calculate(calculation_policy policy, const pixel_buffer_t & pixel_buffer) {
		const auto number_of_pixels{ pixel_buffer.size() };

		histogram_t histogram(256, 0);

		// TODO: return nullopt if invalid pixel input
		//if (number_of_pixels % 32 != 0) { return std::nullopt; }

		if (policy == calculation_policy::cpp) {
			for (auto pixel_value : pixel_buffer) {
				static_assert(sizeof(pixel_value) == 1);
				++histogram[pixel_value];
			}
		} else {
			// masm implementation here
		}
		return std::make_optional(histogram);
	}

	std::optional<std::array<histogram_t,4>> calculate_all(calculation_policy policy, const std::array<pixel_buffer_t, 4> & pixel_buffers) {
		std::array<histogram_t, 4> histograms {};
		for (std::size_t i = 0; i < 4; ++i)
		{
			auto histogram{ calculate(policy, pixel_buffers[i]) };
			if (!histogram) { return std::nullopt; }
			histograms[i] = *histogram;
		}
		return std::make_optional(histograms);
	}
}

MainWidget::MainWidget(MessageProvider provider, QWidget* parent) : QWidget{ parent }, message_provider{ std::move(provider) } {
	build_layout();
	connect_signals();
}

void MainWidget::build_layout() {
	auto layout = new QHBoxLayout{ this };
	auto column = new QVBoxLayout{ this };
	auto strategy_box = new QGroupBox{ "Histogram calculation policy", this };
	auto cpp_strategy_radio = new QRadioButton{ "C++", strategy_box };
	auto asm_strategy_radio = new QRadioButton{ "Assembly", strategy_box };
	auto radio_box_layout = new QHBoxLayout{ this };
	strategy_box->setEnabled(false);

	// buttons
	auto buttons_layout = new QHBoxLayout{ this };
	this->load_image_btn = new QPushButton{ "Load image", this };
	this->load_image_btn->setDefault(true);
	this->generate_histogram_btn = new QPushButton{ "Calculate histogram", this };
	this->generate_histogram_btn->setEnabled(false);
	//this->generate_histogram_btn->setDisabled(true);
	buttons_layout->addWidget(this->load_image_btn);
	buttons_layout->addWidget(this->generate_histogram_btn);

	this->histograms_widget = new HistogramsWidget{ this };
	this->image_label = new QLabel{ this };
	this->image_label->setText(QString{ "No image" });

	radio_box_layout->addWidget(cpp_strategy_radio);
	radio_box_layout->addWidget(asm_strategy_radio);
	strategy_box->setLayout(radio_box_layout);
	cpp_strategy_radio->setChecked(true);
	column->addLayout(buttons_layout);
	column->addWidget(strategy_box);
	column->addSpacing(24);
	//column->addWidget(new QLabel{ "No image", this });
	column->addWidget(this->image_label);
	column->addStretch(1);
	layout->addWidget(this->histograms_widget);
	layout->addLayout(column);
	column->setMargin(16);
}

void MainWidget::connect_signals() {
	connect(this->load_image_btn, SIGNAL(pressed()), this, SLOT(load_image()));
	connect(this->generate_histogram_btn, SIGNAL(pressed()), this, SLOT(calc_histograms()));
	connect(&pixel_buffers_watcher, SIGNAL(finished()), this, SLOT(pixel_buffers_loaded()));
	connect(&histograms_watcher, SIGNAL(finished()), this, SLOT(histograms_calculated()));
}

void MainWidget::pixel_buffers_loaded()
{
	pixel_buffers = std::make_optional(pixel_buffers_watcher.result());
	this->image_label->setVisible(true);
	this->message_provider->show_message(QString{ "Image loaded successfully" },0);
	this->generate_histogram_btn->setEnabled(true);
	this->load_image_btn->setEnabled(true);
}

void MainWidget::histograms_calculated()
{
	auto histograms{ histograms_watcher.result() };
	if (histograms) {
		// std::chrono::duration<double, std::ratio<1, 1000>> millis{ stop - start };
		// this->message_provider->show_message(QString{ "Histograms calculated successfully in " } +QString::number(millis.count()) + QString{ " ms." }, 5'000);
		this->histograms_widget->plot(*histograms);
	} else {
		QMessageBox msg_box{ QMessageBox::NoIcon, QString{"Error"}, QString{"Histogram calculation has failed."}, QMessageBox::Ok, this };
		msg_box.exec();
	}
	this->load_image_btn->setEnabled(true);
	this->generate_histogram_btn->setEnabled(true);
}

void MainWidget::load_image() {
	this->load_image_btn->setEnabled(false);
	QString filename = QFileDialog::getOpenFileName(
		this,
		"Load image",
		QDir::homePath(),
		tr("(*.png *.jpg *.jpeg *.bmp)")
	);
	if (!filename.isEmpty()) {
		QImage image{ filename };
		if (!image.isNull()) {
			image = image.convertToFormat(QImage::Format_RGB32);

			const auto fetch = QtConcurrent::run(load_channels_from_image, image);
			pixel_buffers_watcher.setFuture(fetch);
			
			this->image_label->setVisible(false);
			this->image_label->setPixmap(QPixmap::fromImage(image.scaled(400, 400, Qt::AspectRatioMode::KeepAspectRatio)));
			//this->pixel_buffers = std::make_optional(load_channels_from_image(image));
			
		}
	}
	// this->load_image_btn->setEnabled(true);
}

void MainWidget::calc_histograms() {

		this->generate_histogram_btn->setEnabled(false);

		if (this->pixel_buffers) {
			//auto start{ std::chrono::high_resolution_clock::now() };
			histograms_watcher.setFuture(QtConcurrent::run(calculate_all, calculation_policy::cpp, *this->pixel_buffers));

			//auto histograms { calculate_all(calculation_policy::cpp, *this->pixel_buffers) };
			//auto stop{ std::chrono::high_resolution_clock::now() };

		} else {
			QMessageBox msg_box{ QMessageBox::NoIcon, QString{"Error"}, QString{"Loading pixels from image has failed."}, QMessageBox::Ok, this };
			msg_box.exec();
		}

}
