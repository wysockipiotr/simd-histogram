#include <QMessageBox>
#include <QtWidgets/QApplication>

#include "Utils/Cpuinfo.hpp"
#include "Ui/MainWindow.hpp"
#include "Utils/RuntimeDll.hpp"

namespace {
	auto make_message_box(QStringView title, QStringView content) {
		return std::make_unique<QMessageBox>(
			QMessageBox::NoIcon,
			title.toString(),
			content.toString(),
			QMessageBox::Ok
		);
	}
}

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

	if (cpuinfo::avx_is_supported()) {
		if (runtime_dll::load_dlls()) {
			MainWindow w;
			w.show();
			status_code = a.exec();
		}
		else {
			make_message_box(QString{"Error"}, QString{"Could not load required libraries"})->exec();
			a.quit();
		}
	}
	else {
		make_message_box(QString{"Error"}, QString{"This machine does not support Advanced Vector Extensions"})->exec();
		a.quit();
	}

	return status_code;
}
