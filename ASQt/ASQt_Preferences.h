#pragma once

#include <QWidget>
#include "ui_ASQt_Preferences.h"

class ASQt_Preferences : public QWidget
{
	Q_OBJECT

public:
	ASQt_Preferences(QWidget *parent = nullptr);
	~ASQt_Preferences();

private:
	Ui::ASQt_PreferencesClass ui;
};
