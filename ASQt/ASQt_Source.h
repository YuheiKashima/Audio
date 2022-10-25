#ifndef _ASQTSOURCE_
#define _ASQTSOURCE_

#include "ASQt_Define.h"
#include "ASQt_EQTab.h"

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

	std::shared_ptr<Ui::ASQt_SourceClass> m_spUI;
	std::unique_ptr<ASQt_EQTab> m_upEQUi;

	SourcePlayer m_Source;
	std::vector<std::string> m_FilePaths;

private slots:
	void Play();
	void Stop();
	void Pause();
	void Seeking();
	void Seek();
	void ChangeVolume();
};
#endif