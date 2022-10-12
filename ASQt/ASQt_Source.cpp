#include "ASQt_Source.h"

ASQt_Source::ASQt_Source(QWidget* parent) : QWidget(parent) {
	ui.setupUi(this);
}

ASQt_Source::~ASQt_Source() {
	m_Source.source.reset();
	m_Source.wav.reset();
	m_Source.effect.reset();
}

void ASQt_Source::Open(SourcePlayer _source) {
	m_Source = _source;

	ui.setupUi(this);
	Connect();

	setWindowTitle("AudioSourceTrack");
	setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
	show();
}

void ASQt_Source::dragEnterEvent(QDragEnterEvent* _e) {
	if (_e->mimeData()->hasUrls()) {
		_e->acceptProposedAction();
	}
}

void ASQt_Source::dropEvent(QDropEvent* _e) {
	ui.m_Playlist->clear();
	m_FilePaths.clear();
	m_FilePaths.shrink_to_fit();

	QList<QUrl> fileUrls = _e->mimeData()->urls();
	for (auto i = 0; i < fileUrls.size(); ++i) {
		auto url = fileUrls.at(i).toLocalFile();
		if (url.contains(".wav") == 0 || url.contains(".ogg") == 0) {
			m_FilePaths.push_back(url.toStdString());
			auto pos = m_FilePaths.back().find_last_of("/");
			ui.m_Playlist->addItem(url.right(url.size() - pos - 1));
		}
	}

	if (m_FilePaths.size() > 0)
		ui.m_PlayButton->setEnabled(true);
	else
		ui.m_PlayButton->setEnabled(false);
}

void ASQt_Source::Connect() {
	connect(ui.m_PlayButton, &QPushButton::clicked, this, &ASQt_Source::Play);
	connect(ui.m_StopButton, &QPushButton::clicked, this, &ASQt_Source::Stop);
	connect(ui.m_PauseButton, &QPushButton::clicked, this, &ASQt_Source::Pause);
	connect(ui.m_SourceVol, &QSlider::sliderReleased, this, &ASQt_Source::ChangeVolume);

	if (m_Source.wav)
		ui.m_PlayButton->setEnabled(true);
	else
		ui.m_PlayButton->setEnabled(false);
}

std::shared_ptr<AS::WaveBase> ASQt_Source::GetWaveFromPath(std::string _dir) {
	std::shared_ptr<AS::WaveBase> wave;
	if (_dir.rfind(".wav") != std::string::npos)
		wave = std::make_shared<AS::WavFile>(_dir, AS::EBufferMode::WAVE_BUFFERMODE_STREAM);
	else if (_dir.rfind(".ogg") != std::string::npos)
		wave = std::make_shared<AS::OggFile>(_dir, AS::EBufferMode::WAVE_BUFFERMODE_STREAM);
	return wave;
}

void ASQt_Source::Play() {
	if (m_Source.source->GetState() > AS::EPlayState::AS_PLAYSTATE_STOP) {
		this->Stop();
	}
	m_Source.source->SetEndingCallback([this]() {
		int playIdx = ui.m_Playlist->currentRow();
		if (++playIdx < m_FilePaths.size()) {
			std::string path = m_FilePaths.at(playIdx);
			ui.m_Playlist->setCurrentRow(playIdx);

			m_Source.wav.reset();
			m_Source.wav = GetWaveFromPath(path);

			if (m_Source.wav) {
				ui.m_StopButton->setEnabled(true);
				ui.m_PauseButton->setEnabled(true);
				ui.m_PlayButton->setEnabled(false);
				m_Source.source->Bind(m_Source.wav);
				m_Source.source->PlayShot();
			}
		}
		});

	int playIdx = ui.m_Playlist->currentRow();
	if (playIdx < 0) {
		playIdx = 0;
		ui.m_Playlist->setCurrentRow(0);
	}

	m_Source.wav = GetWaveFromPath(m_FilePaths.at(playIdx));
	ui.m_StopButton->setEnabled(true);
	ui.m_PauseButton->setEnabled(true);
	ui.m_PlayButton->setEnabled(false);
	m_Source.source->Bind(m_Source.wav);
	m_Source.source->PlayShot();
}

void ASQt_Source::Stop() {
	ui.m_StopButton->setEnabled(false);
	ui.m_PauseButton->setEnabled(false);
	ui.m_PauseButton->setText("Pause");
	ui.m_PlayButton->setEnabled(true);
	m_Source.source->Stop();
}

void ASQt_Source::Pause() {
	if (m_Source.source->GetState() >= AS::EPlayState::AS_PLAYSTATE_PLAY) {
		ui.m_StopButton->setEnabled(true);
		ui.m_PauseButton->setEnabled(true);
		ui.m_PauseButton->setText("Resume");
		ui.m_PlayButton->setEnabled(false);
		m_Source.source->Pause();
	}
	else if (m_Source.source->GetState() == AS::EPlayState::AS_PLAYSTATE_PAUSE) {
		ui.m_StopButton->setEnabled(true);
		ui.m_PauseButton->setEnabled(true);
		ui.m_PauseButton->setText("Pause");
		ui.m_PlayButton->setEnabled(false);
		m_Source.source->PlayShot();
	}
}

void ASQt_Source::ChangeVolume() {
	m_Source.source->Volume(static_cast<float>(ui.m_SourceVol->value() / 100.0f));
}