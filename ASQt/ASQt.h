#ifndef _ASQT_
#define _ASQT_

#include <QtWidgets/QMainWindow>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

#include "ASQt_Define.h"
#include "ui_ASQt.h"

class ASQt : public QMainWindow {
	Q_OBJECT
public:
	ASQt(QWidget* parent = nullptr);
	~ASQt();

protected:
	void dragEnterEvent(QDragEnterEvent* _e) override;
	void dropEvent(QDropEvent* _e) override;

private slots:
	void AddSource();

private:
	void ASLaunch();
	void AddSource(QString _dir);

	Ui::ASQtClass ui;
};
#endif