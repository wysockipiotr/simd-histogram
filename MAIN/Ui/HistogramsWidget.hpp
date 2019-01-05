#pragma once

#include "Utils/Shared.hpp"

#include <QWidget>

namespace QtCharts {
	class QChartView;
	class QChart;
} // namespace QtCharts

using chart_bundle_t = std::array<QtCharts::QChartView*, 4>;

class HistogramsWidget : public QWidget {

public:
	explicit HistogramsWidget(QWidget* parent = Q_NULLPTR);

	void plot(const histogram_bundle_t & histograms);

	void clear();
	
private:
	void plot_channel(channel::index ch, const histogram_t& histogram);

	chart_bundle_t chart_views;
};
