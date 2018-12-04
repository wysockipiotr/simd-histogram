#pragma once
#include <QWidget>
#include <unordered_map>
#include "Shared.h"
#include <array>

namespace QtCharts
{
	class QChartView;
	class QChart;
}

class HistogramsWidget : public QWidget {
public:
	HistogramsWidget(QWidget * parent = Q_NULLPTR);

	void plot(const std::array<histogram_t, 4> & histograms);

private:
	void plot_channel(std::size_t ch, const histogram_t & histogram);

	std::array<QtCharts::QChartView *, 4> chart_views;
};

