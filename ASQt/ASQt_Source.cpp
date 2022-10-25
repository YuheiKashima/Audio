#include "ASQt_Source.h"

ASQt_Source::ASQt_Source(QWidget* parent) : QWidget(parent) {
}

ASQt_Source::~ASQt_Source() {
	m_Source.source.reset();
	m_Source.wav.reset();
	m_Source.effect.reset();
}

void ASQt_Source::Open(SourcePlayer _source) {
	m_Source = _source;

	m_spUI = std::make_shared<Ui::ASQt_SourceClass>();
	m_spUI->setupUi(this);

	m_upEQUi = std::make_unique<ASQt_EQTab>(m_spUI, m_Source.equalizer);
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
	m_spUI->m_Playlist->clear();
	m_FilePaths.clear();
	m_FilePaths.shrink_to_fit();

	QList<QUrl> fileUrls = _e->mimeData()->urls();
	for (auto i = 0; i < fileUrls.size(); ++i) {
		auto url = fileUrls.at(i).toLocalFile();
		if (url.contains(".wav") == 0 || url.contains(".ogg") == 0) {
			m_FilePaths.push_back(url.toStdString());
			auto pos = m_FilePaths.back().find_last_of("/");
			m_spUI->m_Playlist->addItem(url.right(url.size() - pos - 1));
		}
	}

	if (m_FilePaths.size() > 0)
		m_spUI->m_PlayButton->setEnabled(true);
	else
		m_spUI->m_PlayButton->setEnabled(false);
}

void ASQt_Source::Connect() {
	connect(m_spUI->m_PlayButton, &QPushButton::clicked, this, &ASQt_Source::Play);
	connect(m_spUI->m_Playlist, &QListView::doubleClicked, this, &ASQt_Source::Play);
	connect(m_spUI->m_StopButton, &QPushButton::clicked, this, &ASQt_Source::Stop);
	connect(m_spUI->m_PauseButton, &QPushButton::clicked, this, &ASQt_Source::Pause);
	connect(m_spUI->m_SourceVol, &QSlider::sliderReleased, this, &ASQt_Source::ChangeVolume);

	if (m_Source.wav)
		m_spUI->m_PlayButton->setEnabled(true);
	else
		m_spUI->m_PlayButton->setEnabled(false);
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
		int playIdx = m_spUI->m_Playlist->currentRow();
		if (++playIdx < m_FilePaths.size()) {
			std::string path = m_FilePaths.at(playIdx);
			m_spUI->m_Playlist->setCurrentRow(playIdx);

			m_Source.wav.reset();
			m_Source.wav = GetWaveFromPath(path);

			if (m_Source.wav) {
				m_spUI->m_StopButton->setEnabled(true);
				m_spUI->m_PauseButton->setEnabled(true);
				m_spUI->m_PlayButton->setEnabled(false);
				m_Source.source->Bind(m_Source.wav);
				m_Source.source->PlayShot();
			}
		}
		else {
			m_spUI->m_StopButton->setEnabled(false);
			m_spUI->m_PauseButton->setEnabled(false);
			m_spUI->m_PlayButton->setEnabled(true);
		}
		});

	int playIdx = m_spUI->m_Playlist->currentRow();
	if (playIdx < 0) {
		playIdx = 0;
		m_spUI->m_Playlist->setCurrentRow(playIdx);
	}

	m_Source.wav = GetWaveFromPath(m_FilePaths.at(playIdx));
	m_spUI->m_StopButton->setEnabled(true);
	m_spUI->m_PauseButton->setEnabled(true);
	m_spUI->m_PlayButton->setEnabled(false);
	m_Source.source->Bind(m_Source.wav);
	m_Source.source->PlayShot();
}

void ASQt_Source::Stop() {
	m_spUI->m_StopButton->setEnabled(false);
	m_spUI->m_PauseButton->setEnabled(false);
	m_spUI->m_PauseButton->setText("Pause");
	m_spUI->m_PlayButton->setEnabled(true);
	m_Source.source->Stop();
}

void ASQt_Source::Pause() {
	if (m_Source.source->GetState() >= AS::EPlayState::AS_PLAYSTATE_PLAY) {
		m_spUI->m_StopButton->setEnabled(true);
		m_spUI->m_PauseButton->setEnabled(true);
		m_spUI->m_PauseButton->setText("Resume");
		m_spUI->m_PlayButton->setEnabled(false);
		m_Source.source->Pause();
	}
	else if (m_Source.source->GetState() == AS::EPlayState::AS_PLAYSTATE_PAUSE) {
		m_spUI->m_StopButton->setEnabled(true);
		m_spUI->m_PauseButton->setEnabled(true);
		m_spUI->m_PauseButton->setText("Pause");
		m_spUI->m_PlayButton->setEnabled(false);
		m_Source.source->PlayShot();
	}
}

void ASQt_Source::ChangeVolume() {
	m_Source.source->Volume(static_cast<float>(m_spUI->m_SourceVol->value() / 100.0f));
}

void ASQt_Source::Seeking() {
}

void ASQt_Source::Seek() {
}