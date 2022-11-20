#include "AudioSystem.h"

#if _DEBUGSINWAVE
float AS::AudioSystem::m_stestSin_a = 0.1f;
float AS::AudioSystem::m_stestSin_f0 = 1000.0f;
uint32_t AS::AudioSystem::m_stestSinWave = 0;
void AS::AudioSystem::testSin(LineBuffer<float>& _buf, const AudioFormat _format) {
	for (uint32_t frames = 0; frames < _buf.sizeX(); ++frames) {
		auto _wave = m_stestSin_a * sin((2.0f * M_PI * m_stestSin_f0 * m_stestSinWave) / _format.samplingRate);
		for (uint32_t chan = 0; chan < _format.channnels; ++chan) {
			_buf[chan][frames] = _wave;
		}
		++m_stestSinWave;
		m_stestSinWave = m_stestSinWave % _format.samplingRate;
	}
}
#endif

#ifdef _WIN32
#include "Wasapi.h"
#endif

AS::AudioSystem::AudioSystem() {
	setlocale(LC_ALL, "Japanese");
}

AS::AudioSystem::~AudioSystem() {
	if (m_upRenderEndPoint->GetState() == EEndPointState::AS_ENDPOINTSTATE_RENDERING) {
		this->Stop(EEndPointMode::AS_ENDPOINTMODE_RENDER);
	}
}

void AS::AudioSystem::RenderThread(std::weak_ptr<MasterTrack> _master) {
	AudioFormat format = m_upRenderEndPoint->GetFormat();
	FramesInfo frames{};
	uint32_t renderFrames = 0;

	while (m_bRenderLoop) {
		frames = FramesInfo();
		renderFrames = 0;

		m_upRenderEndPoint->WaitForProcess();

#if MEASUREMENT_RENDER
		boost::timer::cpu_timer timer;
		timer.start();
#endif

		//要求フレーム数を取得
		m_upRenderEndPoint->GetFrames(frames);
		renderFrames = frames.paddingFrameSize <= 0 ? frames.maxFrameSize : frames.paddingFrameSize;

		LineBuffer<float> renderBuffer(format.channnels, renderFrames);
		renderBuffer.zeroclear();

#if _DEBUGSINWAVE
		testSin(renderBuffer, format);
#else
		//マスタートラックから音声データを収集
		if (auto master = _master.lock()) {
			if (master->GetChildrenSize() > 0) {
				master->GetBuffer(renderBuffer, renderFrames);
			}
		}
#endif

		//エンドポイントへ送信
		m_upRenderEndPoint->Process(renderBuffer, renderFrames);
#if MEASUREMENT_RENDER
		timer.stop();
		m_DebMeasurement.Record(timer.elapsed());

		if (m_DebMeasurement.Count() >= MEASUREMENT_AVERAGE) {
			OutputAverageTime("Render", m_DebMeasurement);
			m_DebMeasurement.Reset();
		}
#endif
	}
	}

void AS::AudioSystem::LaunchDevice(LaunchInfo& _info) {
	std::unique_ptr<EndPointBase> instance;
#ifdef _WIN32
	instance = _info.LaunchDevice.apiName == Wasapi::GetAPIName() ? std::make_unique<Wasapi>() : nullptr;
#endif

	if (instance) {
		instance->LaunchDevice(_info);

		switch (_info.LaunchDevice.endpointMode) {
		case EEndPointMode::AS_ENDPOINTMODE_RENDER:
			if (m_upRenderEndPoint) {
				m_upRenderEndPoint.reset();
			}
			m_upRenderEndPoint = std::move(instance);
			return;
		case EEndPointMode::AS_ENDPOINTMODE_CAPTURE:
			if (m_upCaptureEndPoint) {
				m_upCaptureEndPoint.reset();
			}
			m_upCaptureEndPoint = std::move(instance);
			return;
		default:
			assert(false);
			break;
		}
	}
}

void AS::AudioSystem::SetupDevice(const EEndPointMode _mode, SetupInfo& _info) {
	switch (_mode) {
	case EEndPointMode::AS_ENDPOINTMODE_RENDER:
		if (m_upRenderEndPoint) {
			m_upRenderEndPoint->SetupDevice(_info);
			return;
		}
	default:
		assert(false);
		break;
	}
}

void AS::AudioSystem::Start(const EEndPointMode _mode, StartInfo& _info) {
	switch (_mode) {
	case EEndPointMode::AS_ENDPOINTMODE_RENDER:
		if (m_upRenderEndPoint) {
			m_bRenderLoop = true;
			m_RenderThread = std::thread(&AudioSystem::RenderThread, this, _info.startMaster);
			return m_upRenderEndPoint->Start(_info);
		}
	default:
		assert(false);
		break;
	}
}
void AS::AudioSystem::Stop(const EEndPointMode _mode) {
	switch (_mode) {
	case EEndPointMode::AS_ENDPOINTMODE_RENDER:
		if (m_upRenderEndPoint && m_RenderThread.joinable()) {
			m_bRenderLoop = false;
			m_RenderThread.join();
			return m_upRenderEndPoint->Stop();
		}
	default:
		assert(false);
	}
}

void AS::AudioSystem::Close(const EEndPointMode _mode) {
	switch (_mode) {
	case EEndPointMode::AS_ENDPOINTMODE_RENDER:
		if (m_upRenderEndPoint) {
			m_upRenderEndPoint.reset();
		}
	default:
		assert(false);
	}
}

std::shared_ptr<AS::MasterTrack> AS::AudioSystem::CreateMasterTrack() {
	std::shared_ptr<MasterTrack> none;
	if (!m_upRenderEndPoint)return none;

	EEndPointState state = m_upRenderEndPoint->GetState();
	if (state != EEndPointState::AS_ENDPOINTSTATE_READY && state != EEndPointState::AS_ENDPOINTSTATE_RENDERING)return none;

	AudioFormat format = m_upRenderEndPoint->GetFormat();
	FramesInfo devFrames;
	m_upRenderEndPoint->GetFrames(devFrames);

	return MasterTrack::CreateInstance(format, devFrames.maxFrameSize);
}

std::shared_ptr<AS::SourceTrack> AS::AudioSystem::CreateSourceTrack(std::weak_ptr<MasterTrack> _connectMaster, const uint32_t _bufferTime) {
	std::shared_ptr<SourceTrack> source;
	if (!m_upRenderEndPoint)return source;

	if (auto master = _connectMaster.lock()) {
		source = CreateSourceTrackIndepend(_bufferTime);
		master->ConnectTrack(source);
	}
	return source;
}

std::pair<std::shared_ptr<AS::SourceTrack>, std::shared_ptr<AS::EffectManager>> AS::AudioSystem::CreateSourceTrackWithEffect(std::weak_ptr<MasterTrack> _connectMaster, const uint32_t _bufferTime, const EEffectTiming _effectTiming) {
	std::shared_ptr<SourceTrack> source;
	std::shared_ptr<EffectManager> effect;
	if (!m_upRenderEndPoint)return { source,effect };

	if (auto master = _connectMaster.lock()) {
		EEndPointState state = m_upRenderEndPoint->GetState();
		if (state != EEndPointState::AS_ENDPOINTSTATE_READY && state != EEndPointState::AS_ENDPOINTSTATE_RENDERING)return { source,effect };

		AudioFormat format = m_upRenderEndPoint->GetFormat();
		FramesInfo devFrames;
		m_upRenderEndPoint->GetFrames(devFrames);

		uint32_t createFrames = 0;
		if (_bufferTime > 0)
			createFrames = TimeToFrames(format, _bufferTime);
		else
			createFrames = devFrames.maxFrameSize;

		effect = EffectManager::CreateInstance(format);
		source = SourceTrack::CreateInstance(format, createFrames, _effectTiming, effect);

		master->ConnectTrack(source);
	}
	return { source,effect };
}

std::shared_ptr<AS::SourceTrack> AS::AudioSystem::CreateSourceTrackIndepend(const uint32_t _bufferTime) {
	std::shared_ptr<SourceTrack> source;
	if (!m_upRenderEndPoint)return source;

	EEndPointState state = m_upRenderEndPoint->GetState();
	if (state != EEndPointState::AS_ENDPOINTSTATE_READY && state != EEndPointState::AS_ENDPOINTSTATE_RENDERING)return source;

	AudioFormat format = m_upRenderEndPoint->GetFormat();
	FramesInfo devFrames;
	m_upRenderEndPoint->GetFrames(devFrames);

	uint32_t createFrames = 0;
	if (_bufferTime > 0)
		createFrames = FramesToTime(format, _bufferTime);
	else
		createFrames = devFrames.maxFrameSize;

	source = SourceTrack::CreateInstance(format, createFrames);

	return source;
}