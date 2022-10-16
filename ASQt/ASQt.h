#ifndef _ASQT_
#define _ASQT_

#include <QtWidgets/QMainWindow>

#include "ASQt_Define.h"
#include "ui_ASQt.h"
#include "ASQt_Source.h"

class ASQt : public QMainWindow {
	Q_OBJECT
public:
	ASQt(QWidget* parent = nullptr);
	~ASQt();

protected:
	void dragEnterEvent(QDragEnterEvent* _e) override;
	void dropEvent(QDropEvent* _e) override;

private:
	void ASLaunch();
	void Connect();
	void AddSourceWithWave(QList<QString> _dir);

	Ui::ASQtClass ui;
	AS::AudioSystem m_AudioSystem;
	std::shared_ptr<AS::MasterTrack> m_spMasterTrack;

	std::vector<std::unique_ptr<ASQt_Source>> m_upSources;
private slots:
	void AddSource();
	void ChangeMasterVol();
};
#endif