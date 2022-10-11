#include "ASQt.h"

ASQt::ASQt(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);
}

ASQt::~ASQt() {
}

void ASQt::dragEnterEvent(QDragEnterEvent* _e) {
	if (_e->mimeData()->hasUrls()) {
		_e->acceptProposedAction();
	}
}

void ASQt::dropEvent(QDropEvent* _e) {
	QList<QUrl> fileUrls = _e->mimeData()->urls();
	for (auto i = 0; i < fileUrls.size(); ++i) {
		auto url = fileUrls.at(i).toLocalFile();
		if (url.contains(".wav") == 0 || url.contains(".ogg") == 0) {
		}
	}
}

void ASQt::AddSource() {
}

void ASQt::ASLaunch() {
}

void ASQt::AddSource(QString _dir) {
}