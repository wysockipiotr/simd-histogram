#include "MainWidget.hpp"
#include "HistogramsWidget.hpp"
#include "Utils/Shared.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QTextEdit>

#include <execution>
#include <optional>
#include <chrono>
#include "Utils/DllLoader.hpp"

extern "C" unsigned __int32 NUM_PIXELS_MAX = 4294967295;

namespace {

	auto load_channels_from_image(const QImage& image) {
		const auto number_of_pixels{image.width() * image.height()};

		// create and resize pixel buffer for every channel
		pixel_buffer_bundle_t channel_buffers{};
		for (auto& buffer : channel_buffers) { buffer.reserve(number_of_pixels); }

		// create temporary vector of QRgb values from raw QImage data
		std::vector<QRgb> pixels(number_of_pixels);
		memmove(pixels.data(), image.bits(),
		        number_of_pixels * sizeof(QRgb)); // TODO: memmove or memcpy?

		const auto to_pixel_buffer_bundle{
			[&channel_buffers](const QRgb& pixel)
			{
				channel_buffers[channel::red].push_back(
					static_cast<unsigned __int8>(qRed(pixel)));
				channel_buffers[channel::green].push_back(
					static_cast<unsigned __int8>(qGreen(pixel)));
				channel_buffers[channel::blue].push_back(
					static_cast<unsigned __int8>(qBlue(pixel)));
				channel_buffers[channel::luminance].push_back(
					static_cast<unsigned __int8>(qGray(pixel)));
			}
		};

		// split QRgb data into separate channels of a format suitable for further
		// computation
		std::for_each(std::execution::par_unseq, std::cbegin(pixels),
		              std::cend(pixels), to_pixel_buffer_bundle);

		return channel_buffers;
		// TODO return nullopt if image is incompatible
	}

	auto calculate(calculation_policy policy,
	               const pixel_buffer_t& pixel_buffer) {
		const auto number_of_pixels{pixel_buffer.size()};

		histogram_t histogram(256, 0);

		const auto histogram_function{
			(policy == calculation_policy::cpp)
				? dll_histogram::_cpp_calculate_histogram
				: dll_histogram::_asm_calculate_histogram
		};

		const auto result{
			histogram_function(histogram.data(), pixel_buffer.data(),
			                   number_of_pixels)
		};

		return (result ? std::make_optional(histogram) : std::nullopt);
	}

	std::optional<histogram_bundle_t> calculate_all(
		calculation_policy policy,
		const pixel_buffer_bundle_t& pixel_buffers) {
		histogram_bundle_t histograms{};
		for (std::size_t i = 0; i < 4; ++i) {
			auto histogram{calculate(policy, pixel_buffers[i])};
			if (!histogram) { return std::nullopt; }
			histograms[i] = *histogram;
		}
		return std::make_optional(histograms);
	}

}

MainWidget::MainWidget(MessageProvider provider, QWidget* parent) : QWidget{parent},
                                                                    message_provider{std::move(provider)} {
	build_layout();
	connect_signals();
}

void MainWidget::build_layout() {
	auto layout{new QHBoxLayout{this}};
	auto column{new QVBoxLayout{this}};
	auto strategy_box{new QGroupBox{"Histogram calculation policy", this}};
	auto cpp_strategy_radio{new QRadioButton{"C++", strategy_box}};
	auto asm_strategy_radio{new QRadioButton{"Assembly", strategy_box}};
	auto radio_box_layout{new QHBoxLayout{this}};

	// buttons
	auto buttons_layout{new QHBoxLayout{this}};
	this->load_image_btn = new QPushButton{"Load image", this};
	this->load_image_btn->setDefault(true);
	this->generate_histogram_btn = new QPushButton{"Calculate histogram", this};
	this->generate_histogram_btn->setEnabled(false);
	this->export_histogram_btn = new QPushButton{"Export histogram (*.csv)"};
	this->export_histogram_btn->setEnabled(false);
	buttons_layout->addWidget(this->load_image_btn);
	buttons_layout->addWidget(this->generate_histogram_btn);

	this->histograms_widget = new HistogramsWidget{this};
	this->image_label = new QLabel{this};
	this->image_label->setText(QString{"No image"});

	this->info_text_edit = new QTextEdit{"Some information", this};
	this->info_text_edit->setReadOnly(true);
	this->info_text_edit->setFixedWidth(400);

	radio_box_layout->addWidget(cpp_strategy_radio);
	radio_box_layout->addWidget(asm_strategy_radio);
	strategy_box->setLayout(radio_box_layout);
	cpp_strategy_radio->setChecked(true);

	column->addLayout(buttons_layout);
	column->addWidget(strategy_box);
	column->addSpacing(24);
	column->addWidget(this->image_label);
	column->addStretch(1);
	column->addWidget(this->info_text_edit);
	column->addWidget(this->export_histogram_btn);
	layout->addWidget(this->histograms_widget);
	layout->addLayout(column);
	column->setMargin(16);

	connect(cpp_strategy_radio, &QRadioButton::toggled,
	        [this, cpp_strategy_radio]
	        {
		        this->histograms_widget->clear();
		        this->policy = cpp_strategy_radio->isChecked() ? calculation_policy::cpp : calculation_policy::assembly;
	        });
}

void MainWidget::connect_signals() const {
	connect(this->load_image_btn, SIGNAL(pressed()), this, SLOT(load_image()));
	connect(this->generate_histogram_btn, SIGNAL(pressed()), this, SLOT(calc_histograms()));
	connect(&pixel_buffers_watcher, SIGNAL(finished()), this, SLOT(pixel_buffers_loaded()));
	connect(&histograms_watcher, SIGNAL(finished()), this, SLOT(histograms_calculated()));
}

void MainWidget::pixel_buffers_loaded() {
	pixel_buffers = std::make_optional(pixel_buffers_watcher.result());
	this->image_label->setVisible(true);
	this->message_provider->show_message(QString{"Image loaded successfully"}, 0);
	this->generate_histogram_btn->setEnabled(true);
	this->load_image_btn->setEnabled(true);

	this->histograms_widget->clear();
}

void MainWidget::histograms_calculated() {
	auto histograms{histograms_watcher.result()};
	if (histograms) { this->histograms_widget->plot(*histograms); }
	else {
		QMessageBox msg_box{
			QMessageBox::NoIcon, QString{"Error"}, QString{"Histogram calculation has failed."}, QMessageBox::Ok, this
		};
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
		tr("Image files (*.png *.jpg *.jpeg *.bmp)")
	);

	// filename is empty if user closed file dialog
	if (!filename.isEmpty()) {
		QImage image{filename};
		image = image.convertToFormat(QImage::Format_RGB32);
		this->load_image_btn->setEnabled(true);

		if (!image.isNull()) {
			this->load_image_btn->setEnabled(false);

			const auto fetch = QtConcurrent::run(load_channels_from_image, image);
			pixel_buffers_watcher.setFuture(fetch);

			this->image_label->setVisible(false);
			this->image_label->setPixmap(
				QPixmap::fromImage(image.scaled(400, 400, Qt::AspectRatioMode::KeepAspectRatio)));
			//this->pixel_buffers = std::make_optional(load_channels_from_image(image));
		}
		else {
			QMessageBox msg_box{
				QMessageBox::NoIcon, QString{"Error"},
				QString{"File unsupported."},
				QMessageBox::Ok, this
			};
			msg_box.exec();
		}
	}
	else { this->load_image_btn->setEnabled(true); }
	// this->load_image_btn->setEnabled(true);
}

void MainWidget::calc_histograms() {
	this->histograms_widget->clear();

	this->generate_histogram_btn->setEnabled(false);

	if (this->pixel_buffers) {
		histograms_watcher.setFuture(QtConcurrent::run(calculate_all, this->policy, *this->pixel_buffers));
	}
	else {
		QMessageBox msg_box{
			QMessageBox::NoIcon, QString{"Error"}, QString{"Loading pixels from image has failed."}, QMessageBox::Ok,
			this
		};
		msg_box.exec();
	}
}