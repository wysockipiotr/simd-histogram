#include "MainWindow.h"
#include <QtWidgets/QApplication>
//#include <QLibrary>

//using histogram_calculate_function = __int64(*)(void);

int main(int argc, char *argv[])
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	//QCoreApplication::addLibraryPath(".");
	//QCoreApplication::addLibraryPath("plugins");

	//QLibrary asm_dll{ "DLLASM" };
	//asm_dll.load();

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
