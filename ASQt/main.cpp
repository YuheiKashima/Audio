#include "ASQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	ASQt w;
	w.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint | Qt::MSWindowsFixedSizeDialogHint);
	w.show();
	return a.exec();
}