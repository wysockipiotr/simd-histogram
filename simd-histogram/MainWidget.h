#pragma once

#include <QWidget>
#include <optional>
#include <QFutureWatcher>
#include <QObject>

#include "Shared.h"
#include "MessageProvider.h"

class QPushButton;
class HistogramsWidget;
class QLabel;

class MainWidget : public QWidget {
Q_OBJECT
public:
	explicit MainWidget(MessageProvider provider, QWidget* parent = Q_NULLPTR);

private slots:
	// display image file dialog, load pixels split into channels (rgb + luminance/gray) 
	void load_image();

	void calc_histograms();

	void pixel_buffers_loaded();

	void histograms_calculated();

private:
	void build_layout();

	void connect_signals() const;

	QPushButton*		load_image_btn{};
	QPushButton*		generate_histogram_btn{};
	HistogramsWidget*	histograms_widget{};
	QLabel*				image_label{};

	MessageProvider		message_provider;
    calculation_policy	policy {calculation_policy::cpp};

	QFutureWatcher<pixel_buffer_bundle_t>				pixel_buffers_watcher;
	QFutureWatcher<std::optional<histogram_bundle_t>>	histograms_watcher;

	std::optional<pixel_buffer_bundle_t>				pixel_buffers{ std::nullopt };
};
