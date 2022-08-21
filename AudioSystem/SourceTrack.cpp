#include "SourceTrack.h"

const float AS::SourceTrack::m_sOutLimitDB = -50.0f;
const std::array<std::string, static_cast<size_t>(AS::EPlayState::AS_PLAYSTATE_MAX)> AS::SourceTrack::m_sPlayStateStr{
	"None",
	"Unbind",
	"Stop",
	"Pause",
	"Play",
	"Out"
};

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
	std::lock_guard lock(_request.taskTrack.mutex);

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

	//バッファ書き込み
	for (uint16_t chan = 0; auto & circular : _request.taskTrack.circular) {
		auto src = &loadBuffer.at(chan).front();
		for (uint32_t i = 0; i < _request.orderFrames; ++i) {
			circular.push_back(*src);
			src++;
		}
		chan++;
	}
}

void AS::SourceTrack::CreateBuffer(AudioFormat _format, uint32_t _createFrames) {
	m_Track.circular = std::vector<boost::circular_buffer<float>>(_format.channnels);
	for (auto& buf : m_Track.circular) {
		buf = boost::circular_buffer<float>(_createFrames);
	}

	std::stringstream strstr;
	strstr << "Track\t\t:" << m_InstanceID << std::endl;
	strstr << "Type\t\t:" << m_TrackType << std::endl;
	strstr << "TrackFormat<" << std::endl;
	strstr << "\t" << "Channel\t\t:" << m_Format.channnels << std::endl;
	strstr << "\t" << "SamplingRate\t:" << m_Format.samplingRate << std::endl;
	strstr << "\t" << "BitDepth\t:" << m_Format.bitDepth << std::endl;
	strstr << ">" << std::endl;
	strstr << "CreateFrameSize\t:" << _createFrames << "(" << FramesToTime(_format, _createFrames) << "ms)" << std::endl;

	m_PlayState = EPlayState::AS_PLAYSTATE_UNBIND;
	Log::Logging(Log::ASLOG_INFO, strstr.str());
}

size_t AS::SourceTrack::GetBuffer(LineBuffer<float>& _dest, uint32_t _frames) {
	if (m_PlayState < EPlayState::AS_PLAYSTATE_PLAY)return 0;
	size_t sendFrames = 0, remainFrames = 0, orderFrames = 0;

	if (m_PlayState == EPlayState::AS_PLAYSTATE_PLAY) {
		{
			std::lock_guard lock(m_Track.mutex);
			//使用フレーム数算出(データ長,要求フレーム)
			orderFrames = std::min BOOST_PREVENT_MACRO_SUBSTITUTION(m_Track.circular.front().size(), static_cast<size_t>(_frames));
			//beginから実領域終端と要求フレーム数で小さい方を使用(要求量が終端量より小さいならそのまま送れる)
			sendFrames = std::min BOOST_PREVENT_MACRO_SUBSTITUTION(m_Track.circular.front().array_one().second, orderFrames);
			//残フレームと実領域先頭空endまでの距離を比較し小さいほうを使用
			int32_t remain = 0 >= (orderFrames - sendFrames) ? 0 : orderFrames - sendFrames;
			remainFrames = std::min BOOST_PREVENT_MACRO_SUBSTITUTION(static_cast<size_t>(remain), m_Track.circular.front().array_two().second);

			for (uint32_t chan = 0; auto & buf : m_Track.circular) {
				//リングバッファ先頭ポインタから転送
				std::memcpy(&_dest[chan].front(), buf.array_one().first, sizeof(float) * sendFrames);
				std::memset(buf.array_one().first, NULL, sizeof(float) * sendFrames);
				if (remainFrames > 0) {
					std::memcpy(&_dest[chan][sendFrames], buf.array_two().first, sizeof(float) * remainFrames);
					std::memset(buf.array_two().first, NULL, sizeof(float) * remainFrames);
				}
				//転送した分リングバッファの先頭ポインタを移動(転送分消去)
				buf.erase_begin(sendFrames + remainFrames);
				++chan;
			}
		}

		if (m_Track.is_End) {
			if (m_Track.circular.front().size() <= 0) {
				m_PlayState = EPlayState::AS_PLAYSTATE_OUT;
			}
		}
		else {
			TrackRequest request(*this, m_Track, m_Track.circular.front().capacity() - m_Track.circular.front().size());
			RegisterTask(request);
		}
	}
	else if (m_PlayState == EPlayState::AS_PLAYSTATE_OUT) {
		sendFrames = _frames;
	}

	//エフェクト処理(バッファ送出時処理に設定しているとき)
	if (m_EffectTiming == EEffectTiming::AS_EFFECTTIMING_SENDBUFFER && !m_wpEffectManager.expired()) {
		if (auto effect = m_wpEffectManager.lock())
			effect->Execute(_dest, sendFrames + remainFrames);
	}
	else if (m_wpEffectManager.expired()) {
		m_wpEffectManager.reset();
	}

	//閾値以下の音圧になったら停止
	if (m_PlayState == EPlayState::AS_PLAYSTATE_OUT) {
		auto db = 20 * std::log10f(_dest.max());
		m_PlayState = db <= m_sOutLimitDB ? EPlayState::AS_PLAYSTATE_STOP : EPlayState::AS_PLAYSTATE_OUT;
	}

	if (m_PlayState == EPlayState::AS_PLAYSTATE_STOP) {
		if (auto wav = m_Wave.lock()) {
			wav->Seek(ESeekPoint::WAVE_SEEKPOINT_BEGIN, 0);
		}
		if (m_EndCallback)m_EndCallback();
	}

	//音量調整
	_dest.mul(m_Volume);

	return sendFrames + remainFrames;
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
		else {
			_isEnd = false;
		}
	}
	else {
		m_PlayState = EPlayState::AS_PLAYSTATE_UNBIND;
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
		m_Wave = _wave;
		m_PlayState = EPlayState::AS_PLAYSTATE_STOP;
	}
}

void AS::SourceTrack::Play(PlayOption& _option) {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_STOP) {
		m_Loop = _option.loopCount;
		auto preload = _option.preLoadTime != 0 ? TimeToFrames(m_Format, _option.preLoadTime) : m_Track.circular.front().capacity();
		if (0 < preload && preload <= m_Track.circular.front().capacity()) {
			//途中再生(ms->frames)
			if (auto wav = m_Wave.lock()) {
				uint32_t point = _option.playPoint * m_Format.samplingRate;
				wav->Seek(ESeekPoint::WAVE_SEEKPOINT_BEGIN, point % wav->Size());
			}
			TrackRequest request(*this, m_Track, preload);
			RegisterTask(request);
			m_PlayState = EPlayState::AS_PLAYSTATE_PLAY;
		}
	}
	else if (m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE && m_TempState >= EPlayState::AS_PLAYSTATE_PLAY) {
		m_PlayState = m_TempState;
		m_TempState = EPlayState::AS_PLAYSTATE_NONE;
	}
}

void AS::SourceTrack::PlayShot() {
	PlayOption shot(0);
	this->Play(shot);
}

void AS::SourceTrack::Pause() {
	if (m_PlayState >= EPlayState::AS_PLAYSTATE_PLAY) {
		m_TempState = m_PlayState;
		m_PlayState = EPlayState::AS_PLAYSTATE_PAUSE;
	}
}

void AS::SourceTrack::Stop() {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_PLAY) {
		m_PlayState = EPlayState::AS_PLAYSTATE_OUT;
		if (auto wav = m_Wave.lock()) {
			wav->Seek(ESeekPoint::WAVE_SEEKPOINT_BEGIN, 0);
		}
	}
	else if (m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE || m_PlayState == EPlayState::AS_PLAYSTATE_OUT) {
		m_PlayState = EPlayState::AS_PLAYSTATE_STOP;
		m_TempState = EPlayState::AS_PLAYSTATE_NONE;
		if (auto wav = m_Wave.lock()) {
			wav->Seek(ESeekPoint::WAVE_SEEKPOINT_BEGIN, 0);
		}
	}
}