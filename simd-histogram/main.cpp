#include "Ui/MainWindow.hpp"
#include "Utils/DllLoader.hpp"

#include <QtWidgets/QApplication>
#include <QLibrary>
#include <QMessageBox>

bool load_dlls();

int main(int argc, char* argv[]) {

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	//QCoreApplication::addLibraryPath(".");
	//QCoreApplication::addLibraryPath("plugins");

	QApplication a(argc, argv);

	auto msg_box = new QMessageBox{
		QMessageBox::NoIcon, QString{"Error"},
		QString{"Could not load required library."},
		QMessageBox::Ok
	};

	auto code{0};

	if (load_dlls()) {
		MainWindow w;
		w.show();
		code = a.exec();
	}
	else {
		msg_box->exec();
		a.quit();
	}

	delete msg_box;
	return code;
}

bool load_dlls() {
	QLibrary asm_dll{dll_histogram::asm_library_name};
	QLibrary cpp_dll{dll_histogram::cpp_library_name};

	asm_dll.load();
	cpp_dll.load();

	if (cpp_dll.isLoaded() ) {
		dll_histogram::_asm_calculate_histogram =
			reinterpret_cast<dll_histogram::function>(
				asm_dll.resolve(dll_histogram::asm_function_name));

		dll_histogram::_cpp_calculate_histogram =
			reinterpret_cast<dll_histogram::function>(
				cpp_dll.resolve(dll_histogram::cpp_function_name));
	}

	return ((dll_histogram::_cpp_calculate_histogram != nullptr) && (dll_histogram::_asm_calculate_histogram != nullptr));
}
