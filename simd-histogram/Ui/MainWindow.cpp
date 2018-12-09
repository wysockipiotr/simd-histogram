#include "MainWindow.hpp"
#include <QStatusBar>
#include <QDesktopWidget>
#include <QStyle>
#include <QMessageBox>
#include <QApplication>
#include "Utils/DllLoader.hpp"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent) {

	// set initial window size
	this->resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

	// setup main window status bar
	auto status_bar{new QStatusBar{this}};
	this->setStatusBar(status_bar);

	// create and mount central widget with status bar message provider injected 
	this->widget = new MainWidget{std::make_shared<StatusBarMessageProvider>(status_bar), this};
	setCentralWidget(this->widget);
}
