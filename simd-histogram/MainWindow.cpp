#include "MainWindow.h"
#include <QDialog>
#include <QStatusBar>
#include <QDesktopWidget>
#include <QStyle>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	// set initial window size
	this->resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

	auto status_bar{ new QStatusBar{this} };
	this->setStatusBar(status_bar);
	status_bar->showMessage(QString{ "No image" });

	this->widget = new MainWidget{ std::make_shared<StatusBarMessageProvider>(status_bar), this };
	setCentralWidget(this->widget);
		
}
