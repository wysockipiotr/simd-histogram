#pragma once

#include <QtWidgets/QMainWindow>
#include "MainWidget.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);

private:
	MainWidget * widget;
};
