#pragma once
#include <QWidget>
#include <array>
#include "Shared.h"
#include <optional>
#include <QFutureWatcher>
#include "MessageProvider.h"
#include <QObject>

class QPushButton;
class HistogramsWidget;
class QLabel;


class MainWidget : public QWidget {
	Q_OBJECT
public:
	explicit MainWidget(MessageProvider provider, QWidget * parent = Q_NULLPTR);

private slots:
	// display image file dialog, load pixel values from r, g, b and luminance channels
	void load_image();

	void calc_histograms();

	void pixel_buffers_loaded();

	void histograms_calculated();

private:
	void build_layout();

	void connect_signals();

	QPushButton *			load_image_btn {};
	QPushButton *			generate_histogram_btn {};
	HistogramsWidget *		histograms_widget {};
	QLabel *				image_label {};

	MessageProvider			message_provider;

	QFutureWatcher<std::array<pixel_buffer_t, 4>>				pixel_buffers_watcher;
	QFutureWatcher<std::optional<std::array<histogram_t, 4>>>	histograms_watcher;

	std::optional<std::array<pixel_buffer_t, 4>>				pixel_buffers { std::nullopt };
};
