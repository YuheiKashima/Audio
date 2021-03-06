#pragma once

#include <QtWidgets/QMainWindow>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

#include <AudioSystem.h>
#include <Wasapi.h>
#include <WavFile.h>
#include <Reverb.h>
#include <Equalizer.h>

#include "ui_ASQt.h"

class ASQt : public QMainWindow {
	Q_OBJECT
public:
	ASQt(QWidget* parent = Q_NULLPTR);
	~ASQt();

	bool Lunch() {
		m_Ui.setupUi(this);
		Connect();
		show();
		return true;
	}

protected:
	void dragEnterEvent(QDragEnterEvent* _e) override;
	void dropEvent(QDropEvent* _e) override;
private slots:
	void Play();
	void Stop();
	void Pause();
	void ChangeVol();
	void OpenReverbWindow();
	void OpenEqualizerWindow();
private:
	struct Source {
		std::shared_ptr<AS::SourceTrack> source;
		std::shared_ptr<AS::WavFile> wav;
		std::shared_ptr<AS::EffectManager> effect;
		std::weak_ptr<AS::Reverb> reverb;
		std::weak_ptr<AS::Equalizer> equalizer;
	};

	void Connect();
	Source CreateSource();

	std::unique_ptr<AS::AudioSystem> m_AudioSystem;
	AS::DeviceList m_DevList;
	std::shared_ptr<AS::MasterTrack> m_spMasterTrack;
	Source m_Source;

	Ui::ASQtClass m_Ui;
	std::vector<std::string> m_FilePaths;
};
