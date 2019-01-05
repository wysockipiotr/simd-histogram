#include <QtWidgets/QApplication>

#include <QMessageBox>
#include "Utils/RuntimeDll.hpp"
#include "Ui/MainWindow.hpp"

int main(int argc, char** argv) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

	QApplication a{argc, argv};

	auto msg_box{
		std::make_unique<QMessageBox>(
			QMessageBox::NoIcon, QString{"Error"},
			QString{"Could not load required libraries."}, QMessageBox::Ok)
	};

	auto status_code{0};

	if (runtime_dll::load_dlls()) {
		MainWindow w;
		w.show();
		status_code = a.exec();
	}
	else {
		msg_box->exec();
		a.quit();
	}

	return status_code;
}
