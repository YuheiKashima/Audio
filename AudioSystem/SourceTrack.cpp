#include "SourceTrack.h"

AS::SourceTrack::SourceTrack(AudioFormat _format, uint32_t _createFrames) :TrackBase(_format, "SourceTrack"), m_PlayState(EPlayState::AS_PLAYSTATE_UNBIND) {
	CreateBuffer(_format, _createFrames);
}

AS::SourceTrack::SourceTrack(AudioFormat _format, uint32_t _createFrames, EEffectTiming _effectTiming, std::weak_ptr<EffectManager> _effectManager) : TrackBase(_format, "SourceTrack") {
	CreateBuffer(_format, _createFrames);

	std::stringstream strstr;
	if (!_effectManager.expired()) {
		m_wpEffectManager = _effectManager;
		m_EffectTiming = _effectTiming;
		strstr << "Connect EffectManager" << std::endl;
	}

	Log::Logging(Log::ASLOG_INFO, strstr.str(), false);
}

AS::SourceTrack::~SourceTrack() {
}

void AS::SourceTrack::TaskProcess(TrackRequest& _request) {
	size_t filling = 0;
	LineBuffer<float> loadBuffer(m_Format.channnels, _request.orderFrames);

	filling = Load(loadBuffer, _request.orderFrames, _request.taskTrack.is_End);

	//エフェクト処理(IO時処理に設定しているとき)
	if (m_EffectTiming == EEffectTiming::AS_EFFECTTIMING_IO && !m_wpEffectManager.expired()) {
		if (auto effect = m_wpEffectManager.lock())
			effect->Execute(loadBuffer, filling);
	}
	else if (m_wpEffectManager.expired()) {
		m_wpEffectManager.reset();
	}

	std::lock_guard lock(_request.taskTrack.mutex);
#if USE_CIRCULAR
	for (uint16_t chan = 0; auto & circular : _request.taskTrack.circular) {
		auto src = &loadBuffer.at(chan).front();
		for (uint32_t i = 0; i < _request.orderFrames; ++i) {
			circular.push_back(*src);
			src++;
		}
		chan++;
	}
#else
	_request.taskTrack.buffer = loadBuffer;
	_request.taskTrack.fillingBuffer = filling;
#endif
}

void AS::SourceTrack::CreateBuffer(AudioFormat _format, uint32_t _createFrames) {
#if USE_CIRCULAR
	m_Track.circular = std::vector<boost::circular_buffer<float>>(_format.channnels);
	for (auto& buf : m_Track.circular) {
		buf = boost::circular_buffer<float>(_createFrames);
	}
#else
	for (auto& track : m_Tracks) {
		track.buffer.resize(m_Format.channnels, _createFrames);
	}
#endif

	std::stringstream strstr;
	strstr << "Track\t\t:" << m_InstanceID << std::endl;
	strstr << "Type\t\t:" << m_TrackType << std::endl;
	strstr << "TrackFormat<" << std::endl;
	strstr << "\t" << "Channel\t\t:" << m_Format.channnels << std::endl;
	strstr << "\t" << "SamplingRate\t:" << m_Format.samplingRate << std::endl;
	strstr << "\t" << "BitDepth\t:" << m_Format.bitDepth << std::endl;
	strstr << ">" << std::endl;
	strstr << "CreateFrameSize\t:" << _createFrames << std::endl;

	m_PlayState = EPlayState::AS_PLAYSTATE_UNBIND;
	Log::Logging(Log::ASLOG_INFO, strstr.str());
}

size_t AS::SourceTrack::GetBuffer(LineBuffer<float>& _dest, uint32_t _frames) {
	if (m_PlayState != EPlayState::AS_PLAYSTATE_PLAY)return 0;

#if USE_CIRCULAR
	//使用フレーム数算出(データ長,要求フレーム)
	size_t sendFrames = 0, remainFrames = 0, orderFrames = std::min(m_Track.circular.front().size(), static_cast<size_t>(_frames));
	//beginから実領域終端と要求フレーム数で小さい方を使用(要求量が終端量より小さいならそのまま送れる)
	sendFrames = std::min(m_Track.circular.front().array_one().second, orderFrames);
	//残フレームと実領域先頭空endまでの距離を比較し小さいほうを使用
	int32_t remain = 0 >= (orderFrames - sendFrames) ? 0 : orderFrames - sendFrames;
	remainFrames = std::min(static_cast<size_t>(remain), m_Track.circular.front().array_two().second);

	{
		std::lock_guard lock(m_Track.mutex);
		for (uint32_t chan = 0; auto & buf : m_Track.circular) {
			//リングバッファ先頭ポインタから転送
			std::memcpy(&_dest[chan].front(), buf.array_one().first, sizeof(float) * sendFrames);
			if (remainFrames > 0) {
				std::memcpy(&_dest[chan][sendFrames], buf.array_two().first, sizeof(float) * remainFrames);
			}
			//転送した分リングバッファの先頭ポインタを移動(転送分消去)
			buf.erase_begin(sendFrames + remainFrames);

			++chan;
		}
	}

	if (m_Track.is_End) {
		if (m_Track.circular.front().size() <= 0) {
			if (m_EndCallback)m_EndCallback();
			Stop();
		}
	}
	else {
		TrackRequest request(*this, m_Track, sendFrames + remainFrames);
		RegisterTask(request);
	}

#else
	auto& primary = m_Tracks.at(m_UseTrack);

	//使用フレーム数算出
	size_t sendFrames = 0, remainFrames = 0, maxFrames = std::min(static_cast<size_t>(primary.fillingBuffer), primary.buffer.sizeX());
	if (m_Cuesor + _frames < maxFrames) {
		sendFrames = _frames;
	}
	else {
		sendFrames = maxFrames - m_Cuesor;
		remainFrames = primary.is_End ? 0 : _frames - sendFrames;
	}

	{
		//クリティカルセクション(プライマリバッファ)
		//プライマリバッファから送出
		std::lock_guard lock(primary.mutex);
		for (uint32_t c = 0; c < m_Format.channnels; ++c) {
			std::memcpy(&_dest[c].front(), &primary.buffer[c][m_Cuesor], sizeof(float) * sendFrames);
		}
		m_Cuesor += (uint32_t)sendFrames;
	}

	if (primary.is_End && !remainFrames ? true : false) {
		if (m_EndCallback)m_EndCallback();
		Stop();
	}
	else if (remainFrames > 0) {
		//残バッファをセカンダリバッファから送出
		auto& secondry = m_Tracks.at((m_UseTrack + 1) % static_cast<uint32_t>(ETrackNum::DOUBLE_MAX));

		TrackRequest request(*this, primary, primary.buffer.sizeX());
		RegisterTask(request);

		//クリティカルセクション(セカンダリバッファ)
		std::lock_guard lock(secondry.mutex);
		for (uint32_t c = 0; c < m_Format.channnels; ++c) {
			std::memcpy(&_dest[c][sendFrames], &secondry.buffer[c].front(), sizeof(float) * remainFrames);
		}
		m_Cuesor = (uint32_t)remainFrames;
		//使用バッファ切り替え
		m_UseTrack = (m_UseTrack + 1) % static_cast<uint32_t>(ETrackNum::DOUBLE_MAX);
	}
#endif

	//エフェクト処理(バッファ送出時処理に設定しているとき)
	if (m_EffectTiming == EEffectTiming::AS_EFFECTTIMING_SENDBUFFER && !m_wpEffectManager.expired()) {
		if (auto effect = m_wpEffectManager.lock())
			effect->Execute(_dest, sendFrames + remainFrames);
	}
	else if (m_wpEffectManager.expired()) {
		m_wpEffectManager.reset();
	}

	//音量調整
	_dest.avx_mul(m_Volume);

	return sendFrames + remainFrames;
}

//プレロード(事前バッファ満たし)
void AS::SourceTrack::PreLoad() {
#if USE_CIRCULAR
	m_Track.is_End = false;

	{
		std::lock_guard lock(m_Track.mutex);
		for (auto& cir : m_Track.circular) {
			cir.clear();
		}
	}

	TrackRequest request(*this, m_Track, m_Track.circular.front().capacity());
	RegisterTask(request);
#else
	m_Cuesor = 0;
	m_UseTrack = static_cast<uint32_t>(ETrackNum::PRIMARY);

	for (auto& track : m_Tracks) {
		std::lock_guard lock(track.mutex);
		track.buffer.zeroclear();
		track.fillingBuffer = 0;
		track.is_End = false;
	}

	auto& pri = m_Tracks.at(static_cast<size_t>(ETrackNum::PRIMARY));
	auto& sec = m_Tracks.at(static_cast<size_t>(ETrackNum::SECONDRY));

	{
		TrackRequest request(*this, pri, pri.buffer.sizeX());
		RegisterTask(request);
	}
	{
		TrackRequest request(*this, sec, sec.buffer.sizeX());
		RegisterTask(request);
	}
#endif
}

//Waveからロード
size_t AS::SourceTrack::Load(LineBuffer<float>& _dest, size_t _loadFrames, bool& _isEnd) {
	size_t filling = 0;
	if (auto wav = m_Wave.lock()) {
		bool is_end = false, loop = m_Loop > 0 ? true : false;
		//バッファゲッチュ！！
		filling = wav->GetBuffer(_dest, (uint32_t)_loadFrames, loop, is_end);

		//終了判定
		if (is_end) {
			if (m_Loop > 0) {
				_isEnd = false;
				--m_Loop;
			}
			else if (m_Loop <= 0) {
				_isEnd = true;
			}
		}
	}
	else {
		Stop();
	}
	return filling;
}

void AS::SourceTrack::Bind(std::weak_ptr<WaveBase> _wave) {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_NONE) {
		return;
	}

	if (m_PlayState == EPlayState::AS_PLAYSTATE_PLAY ||
		m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE) {
		this->Stop();
	}

	if (m_PlayState == EPlayState::AS_PLAYSTATE_STOP ||
		m_PlayState == EPlayState::AS_PLAYSTATE_UNBIND) {
#if USE_CIRCULAR
#else
		m_UseTrack = static_cast<uint32_t>(ETrackNum::PRIMARY);
#endif
			m_Wave = _wave;
			PreLoad();
			m_PlayState = EPlayState::AS_PLAYSTATE_STOP;
}
}

void AS::SourceTrack::Play(PlayOption& _option) {
	m_Loop = _option.loopCount;

	if (m_PlayState == EPlayState::AS_PLAYSTATE_STOP || m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE)
		m_PlayState = EPlayState::AS_PLAYSTATE_PLAY;
}

void AS::SourceTrack::PlayShot() {
	PlayOption shot(0);
	this->Play(shot);
}

void AS::SourceTrack::Pause() {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_PLAY)
		m_PlayState = EPlayState::AS_PLAYSTATE_PAUSE;
}

void AS::SourceTrack::Stop() {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_PLAY || m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE) {
		m_PlayState = EPlayState::AS_PLAYSTATE_STOP;
		if (auto wav = m_Wave.lock()) {
			wav->Seek(ESeekPoint::WAVE_SEEKPOINT_BEGIN, 0);
		}

		PreLoad();
	}
}