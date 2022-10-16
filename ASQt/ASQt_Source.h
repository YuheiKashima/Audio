#ifndef _ASQTSOURCE_
#define _ASQTSOURCE_

#include <QWidget>

#include "ASQt_Define.h"
#include "ui_ASQt_Source.h"

class ASQt_Source : public QWidget {
	Q_OBJECT
public:
	ASQt_Source(QWidget* parent = nullptr);
	~ASQt_Source();

	void Open(SourcePlayer _source);
protected:
	void dragEnterEvent(QDragEnterEvent* _e)override;
	void dropEvent(QDropEvent* _e)override;

private:
	void Connect();
	std::shared_ptr<AS::WaveBase> GetWaveFromPath(std::string _dir);

	Ui::ASQt_SourceClass ui;
	SourcePlayer m_Source;
	std::vector<std::string> m_FilePaths;

private slots:
	void Play();
	void Stop();
	void Pause();
	void ChangeVolume();
	void ChangeEqParam();
	void ChangeEqEnable();
	void ChangeReverbParam();
	void ChangeReverbEnable();
	void ChangeCompParam();
	void ChangeCompEnable();
};
#endif