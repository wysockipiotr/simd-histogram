#include <QColor>

#include <QVBoxLayout>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "HistogramsWidget.hpp"
#include "Utils/Shared.hpp"

namespace {
	const QColor MATERIAL_RED{0xF4, 0x43, 0x3};
	const QColor MATERIAL_GREEN{0x4C, 0xAF, 0x50};
	const QColor MATERIAL_BLUE{0x21, 0x96, 0xF3};
	const QColor MATERIAL_GRAY{0x9E, 0x9E, 0x9E};
	const std::array<const QString, 4> HISTOGRAM_NAMES{"Red", "Green", "Blue", "Luminance"};
} // namespace

HistogramsWidget::HistogramsWidget(QWidget* parent)
	: QWidget(parent), chart_views{} {
	// column of histogram chart widgets
	auto column = new QVBoxLayout{this};

	for (channel::index ch{0}; ch < channel::end; ++ch) {
		auto chart_view = new QtCharts::QChartView{this};
		chart_views[ch] = chart_view;
		chart_view->setChart(new QtCharts::QChart);
		chart_view->setRenderHint(QPainter::Antialiasing);
		chart_view->setToolTip(HISTOGRAM_NAMES[ch] + " histogram");
		column->addWidget(chart_view);
	}
	setLayout(column);
}

void HistogramsWidget::plot(const std::array<histogram_t, 4>& histograms) {
	for (channel::index ch{0}; ch < channel::end; ++ch) { plot_channel(ch, histograms[ch]); }
}

void HistogramsWidget::plot_channel(channel::index ch,
                                    const histogram_t& histogram) {
	// histogram (upper line)
	auto series{new QtCharts::QLineSeries{this}};

	// bottom straight line
	auto bottom_series{new QtCharts::QLineSeries{this}};

	// populate series
	Q_ASSERT(histogram.size() == 256);
	for (auto i{0}; i < 256; ++i) {
		series->append(i, histogram[i]);
		bottom_series->append(i, 0);
	}

	// merge line series into area
	auto area_series{new QtCharts::QAreaSeries{series, bottom_series}};

	// plot
	const auto widget{this->chart_views[ch]};
	widget->chart()->legend()->hide();
	widget->chart()->removeAllSeries();
	widget->chart()->addSeries(area_series);

	switch (ch) {
	case channel::red:
		area_series->setColor(MATERIAL_RED);
		widget->chart()->setTitle("Red");
		break;
	case channel::green:
		area_series->setColor(MATERIAL_GREEN);
		widget->chart()->setTitle("Green");
		break;
	case channel::blue:
		area_series->setColor(MATERIAL_BLUE);
		widget->chart()->setTitle("Blue");
		break;
	case channel::luminance:
		area_series->setColor(MATERIAL_GRAY);
		widget->chart()->setTitle("Luminance");
		break;
	default:
		break;
	}
}

void HistogramsWidget::clear() {
	for (channel::index i{0}; i < channel::end; ++i) { this->chart_views[i]->chart()->removeAllSeries(); }
}
