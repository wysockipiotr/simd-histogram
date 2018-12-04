#include <QVBoxLayout>
#include <QColor>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include "Shared.h"
#include "HistogramsWidget.h"

namespace {
	const QColor MATERIAL_RED{ 0xF4, 0x43, 0x3 };
	const QColor MATERIAL_GREEN{ 0x4C, 0xAF, 0x50 };
	const QColor MATERIAL_BLUE{ 0x21, 0x96, 0xF3 };
	const QColor MATERIAL_GRAY{ 0x9E, 0x9E, 0x9E };
}

HistogramsWidget::HistogramsWidget(QWidget * parent) : QWidget(parent), chart_views{} {
	auto column = new QVBoxLayout{ this };

	for (std::size_t ch = 0; ch < 4; ++ch) {
		auto chart_view = new QtCharts::QChartView{ this };
		chart_views[ch] = chart_view;
		chart_view->setChart(new QtCharts::QChart);
		chart_view->setRenderHint(QPainter::Antialiasing);
		column->addWidget(chart_view);
	}
	setLayout(column);
}

void HistogramsWidget::plot(const std::array<histogram_t, 4> & histograms) {
	for (std::size_t ch = 0; ch < 4; ++ch) {
		plot_channel(ch, histograms[ch]);
	}
}

void HistogramsWidget::plot_channel(std::size_t ch, const histogram_t & histogram) {
	auto series{ new QtCharts::QLineSeries };
	auto bottom_series{ new QtCharts::QLineSeries };
	for (int i{ 0 }; i < 256; ++i) {
		series->append(i, histogram[i] + 1);
		bottom_series->append(i, 0);
	}
	auto area_series{ new QtCharts::QAreaSeries{ series, bottom_series } };
	auto widget{ this->chart_views[ch] };
	widget->chart()->legend()->hide();
	widget->chart()->removeAllSeries();
	//widget->chart()->zoomReset();
	widget->chart()->addSeries(area_series);

	switch (ch) {
	case channel::red:
		area_series->setColor(MATERIAL_RED); 
		widget->chart()->setTitle("R"); break;
	case channel::green:
		area_series->setColor(MATERIAL_GREEN);
		widget->chart()->setTitle("G"); break;
	case channel::blue:
		area_series->setColor(MATERIAL_BLUE); widget->chart()->setTitle("B"); break;
	case channel::luminance:
		area_series->setColor(MATERIAL_GRAY); widget->chart()->setTitle("Luminance"); 
		break;
	default: break;
	}
	//widget->chart()->zoomReset();
	// auto max_element = std::max_element(histogram.begin(), histogram.end());
	// if (max_element != histogram.end())
	// {
		// widget->chart()->setAxisY(new QtCharts::QValueAxis(widget->chart()));
		// widget->chart()->axisY()->setRange(0, 1000);
		//widget->chart()->axisY()->setMax(*max_element);
	// }
}