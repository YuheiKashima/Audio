#include "ASQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	ASQt w;
	w.show();
	return a.exec();
}