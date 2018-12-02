#include "MainWindow.h"
#include "ChannelPixels.h"
#include "Histogram.h"
#include <QDialog>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	Histogram histogram{};
	ChannelPixels pixels{};
	pixels.load_image();
	histogram.use_asm(true);
	
	auto status{ new QStatusBar{this} };
	status->showMessage(QString{ (histogram.calculate(pixels) ? "TAK" : "NIE") });
	this->setStatusBar(status);
}
