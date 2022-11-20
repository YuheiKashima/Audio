#include "ASQt.h"

ASQt::ASQt(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);
	ASLaunch();
	Connect();
}

ASQt::~ASQt() {
	m_AudioSystem.Stop(AS::EEndPointMode::AS_ENDPOINTMODE_RENDER);
}

void ASQt::dragEnterEvent(QDragEnterEvent* _e) {
	if (_e->mimeData()->hasUrls()) {
		//_e->acceptProposedAction();
	}
}

void ASQt::dropEvent(QDropEvent* _e) {
	QList<QUrl> fileUrls = _e->mimeData()->urls();
	for (auto i = 0; i < fileUrls.size(); ++i) {
		auto url = fileUrls.at(i).toLocalFile();
		if (url.contains(".wav") == 0 || url.contains(".ogg") == 0) {
		}
	}
}

void ASQt::ASLaunch() {
	myLib::Log::Open(true, AS::Log::ASLOG_ALL);
	myLib::Log::SetLogOutputCallback([this](std::string _log) {
		ui.m_LogView->append(QString::fromStdString(_log));
		});

	AS::DeviceInfo devInfo("", AS::Wasapi::GetAPIName(), AS::EEndPointMode::AS_ENDPOINTMODE_RENDER);
	AS::AudioFormat audFormat(48000, 16, 2), altFormat;
	AS::WasapiLaunchInfo launch(devInfo, audFormat, AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, &altFormat);
	m_AudioSystem.LaunchDevice(launch);

	AS::WasapiSetupInfo setup(0, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK);
	m_AudioSystem.SetupDevice(AS::EEndPointMode::AS_ENDPOINTMODE_RENDER, setup);

	m_spMasterTrack = m_AudioSystem.CreateMasterTrack();

	AS::WasapiStartInfo start(m_spMasterTrack, 2000);
	m_AudioSystem.Start(AS::EEndPointMode::AS_ENDPOINTMODE_RENDER, start);
}

void ASQt::Connect() {
	connect(ui.m_AddButton, &QPushButton::clicked, this, &ASQt::AddSource);
	connect(ui.m_MasterVol, &QSlider::sliderReleased, this, &ASQt::ChangeMasterVol);
}

void ASQt::AddSourceWithWave(QList<QString> _dir) {
}

void ASQt::AddSource() {
	auto sourceEffect = m_AudioSystem.CreateSourceTrackWithEffect(m_spMasterTrack, 0, AS::EEffectTiming::AS_EFFECTTIMING_SENDBUFFER);

	SourcePlayer source;
	source.source = sourceEffect.first;
	source.effect = sourceEffect.second;
	source.reverb = source.effect->AddEffect<AS::Reverb>();
	source.equalizer = source.effect->AddEffect<AS::Equalizer>();
	auto eqparam = AS::EQParam(
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f
	);
	source.equalizer.lock()->SetEffectParam(eqparam);
	source.compressor = source.effect->AddEffect<AS::Compressor>();

	auto asqtSource = std::make_unique<ASQt_Source>(this);
	asqtSource->setAttribute(Qt::WA_DeleteOnClose);
	asqtSource->Open(source);
	m_upSources.push_back(std::move(asqtSource));
}

void ASQt::ChangeMasterVol() {
	m_spMasterTrack->Volume(static_cast<float>(ui.m_MasterVol->value() / 100.0f));
}