#pragma once

#include <QWidget>

#include "ASQt_Define.h"
#include "ui_ASQt_Source.h"

class ASQt_Source : public QWidget {
	Q_OBJECT

public:
	ASQt_Source(QWidget* parent = nullptr);
	~ASQt_Source();

private:
	Ui::ASQt_SourceClass ui;
};
