#include "ASQt.h"

ASQt::ASQt(QWidget* parent) : QMainWindow(parent) {
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
	for (uint32_t i = 0; i < fileUrls.size(); ++i) {
		auto url = fileUrls.at(i).toLocalFile();
		if (url.contains(".wav") == 0 || url.contains(".ogg") == 0) {
			m_FilePaths.push_back(url.toStdString());
			size_t pos = m_FilePaths.back().find_last_of("//");
			m_Ui.m_PlayList->addItem(QString::fromStdString(m_FilePaths.back().substr(pos + 1)));
		}
	}
}

void ASQt::keyPressEvent(QKeyEvent* _e) {
	if (_e->key() == Qt::Key::Key_Delete) {
		QList<QListWidgetItem*> selectItems = m_Ui.m_PlayList->selectedItems();
		m_Ui.m_PlayList->currentRow();
		qDeleteAll(selectItems);
	}
}

void ASQt::Connect() {
	connect(m_Ui.m_PlayButton, &QPushButton::clicked, this, &ASQt::Play);
	connect(m_Ui.m_PauseButton, &QPushButton::clicked, this, &ASQt::Pause);
	connect(m_Ui.m_StopButton, &QPushButton::clicked, this, &ASQt::Stop);
	//connect(m_Ui.m_AudioList, &QListWidget::doubleClicked, this, &ASQt::Play);
	connect(m_Ui.m_VolController, &QSlider::sliderReleased, this, &ASQt::ChangeVol);
	//connect(m_Ui.m_ActionReverb, &QAction::triggered, this, &ASQt::OpenReverbWindow);
	//connect(m_Ui.m_ActionEQ, &QAction::triggered, this, &ASQt::OpenEqualizerWindow);
}

void ASQt::LaunchAS() {
	AS::AudioSystem::GetInstance().EnumerateDevices<AS::Wasapi>(AS::EEndPointMode::AS_ENDPOINTMODE_RENDER, m_DevList);
	AS::AudioFormat altFormat;
	AS::WasapiLaunchInfo Launch(AS::DeviceInfo("", AS::Wasapi::GetAPIName()), AS::AudioFormat(48000, 16, 2), AUDCLNT_SHAREMODE_SHARED, &altFormat);
	AS::AudioSystem::GetInstance().LaunchDevice(Launch);
	AS::WasapiSetupInfo setup(0, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK);
	AS::AudioSystem::GetInstance().SetupDevice(AS::EEndPointMode::AS_ENDPOINTMODE_RENDER, setup);
	m_spMasterTrack = AS::AudioSystem::GetInstance().CreateMasterTrack();

	AS::WasapiStartInfo start(m_spMasterTrack, 2000);
	AS::AudioSystem::GetInstance().Start(AS::EEndPointMode::AS_ENDPOINTMODE_RENDER, start);

	m_Source = CreateSource();
}

ASQt::Source ASQt::CreateSource() {
	Source s;
	auto [source, effect] = AS::AudioSystem::GetInstance().CreateSourceTrackWithEffect(m_spMasterTrack, 0, AS::EEffectTiming::AS_EFFECTTIMING_SENDBUFFER);
	s.reverb = effect->AddEffect<AS::Reverb>();
	s.equalizer = effect->AddEffect<AS::Equalizer>();
	s.effect = effect; s.source = source;
	return s;
}

void ASQt::Play() {
	Stop();
	m_Source.source->SetEndingCallback([this]() {
		uint32_t currentRow = m_Ui.m_PlayList->currentRow();
		if (++currentRow < m_Ui.m_PlayList->count()) {
			m_Ui.m_PlayList->setCurrentRow(currentRow);
			m_Source.wav.reset();
			m_Source.wav = std::make_shared<AS::WavFile>(m_FilePaths[currentRow], AS::EBufferMode::WAVE_BUFFERMODE_STREAM);
			m_Source.source->Bind(m_Source.wav);
			m_Ui.m_StopButton->setEnabled(true);
			m_Ui.m_PauseButton->setEnabled(true);
			m_Ui.m_PlayButton->setEnabled(false);
			m_Source.source->PlayShot();
		}
		});

	uint32_t playIdx = m_Ui.m_PlayList->currentRow();
	m_Source.wav.reset();
	m_Source.wav = std::make_shared<AS::WavFile>(m_FilePaths[playIdx], AS::EBufferMode::WAVE_BUFFERMODE_STREAM);
	m_Source.source->Bind(m_Source.wav);
	m_Ui.m_StopButton->setEnabled(true);
	m_Ui.m_PauseButton->setEnabled(true);
	m_Ui.m_PlayButton->setEnabled(false);
	AS::PlayOption opt(AS::LoopInfinity);
	m_Source.source->Play(opt);
}

void ASQt::Stop() {
	m_Ui.m_StopButton->setEnabled(false);
	m_Ui.m_PauseButton->setEnabled(false);
	m_Ui.m_PlayButton->setEnabled(true);
	m_Source.source->Stop();
}

void ASQt::Pause() {
	m_Ui.m_StopButton->setEnabled(true);
	m_Ui.m_PauseButton->setEnabled(false);
	m_Ui.m_PlayButton->setEnabled(true);
	m_Source.source->Pause();
}

void ASQt::ChangeVol() {
	m_Source.source->Volume(static_cast<float>(m_Ui.m_VolController->value() / 100.0f));
}

void ASQt::OpenReverbWindow() {
}

void ASQt::OpenEqualizerWindow() {
}