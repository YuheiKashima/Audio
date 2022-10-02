#include "ASQt.h"
#include <QtWidgets/QApplication>

#pragma comment(lib,"AudioSystem.lib")

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	ASQt w;
	return w.Launch() ? a.exec() : 0;
}