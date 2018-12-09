#pragma once

#include <QtWidgets/QMainWindow>

#include "MainWidget.hpp"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = Q_NULLPTR);

private:
	MainWidget* widget;
};
