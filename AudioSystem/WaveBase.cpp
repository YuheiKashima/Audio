#include "WaveBase.h"

AS::WaveBase::WaveBase() {
}

AS::WaveBase::~WaveBase() {
	if (m_WaveStream.is_open()) {
		m_WaveStream.close();
	}
}

size_t AS::WaveBase::GetBuffer(LineBuffer<float>& _dest, uint32_t _frames, const bool _loopFlg, bool& _isEnd) {
	if (_dest.sizeX() < _frames) {
		return 0;
	}
	//ロック
	std::lock_guard<std::recursive_mutex> lock(m_WavMutex);

	_dest.zeroclear();
	uint32_t useFrames = 0;
	switch (m_BufMode) {
	case EBufferMode::WAVE_BUFFERMODE_LOADALL:
		return GetMemory(_dest, _frames, _loopFlg, _isEnd);
		break;
	case EBufferMode::WAVE_BUFFERMODE_STREAM:
		return GetStream(_dest, _frames, _loopFlg, _isEnd);
		break;
	default:
		return 0;
	}
}

size_t AS::WaveBase::GetMemory(LineBuffer<float>& _dest, uint32_t _frames, const bool _loopFlg, bool& _isEnd) {
	auto sendFrames = 0, remainFrames = 0;
	if ((m_Cursor + _frames) < m_AllFrames) {
		sendFrames = _frames;
		_isEnd = false;
	}
	else {
		sendFrames = m_AllFrames - m_Cursor;
		remainFrames = _loopFlg ? _frames - sendFrames : 0;
		_isEnd = true;
	}

	for (uint32_t c = 0; c < m_Format.channnels; ++c) {
		std::memcpy(&_dest[c].front(), &m_RealWave[c][m_Cursor], sizeof(float) * sendFrames);
	}

	if (_isEnd) {
		//どちらにしろ頭には移動するのでシーク。
		Seek(ESeekPoint::WAVE_SEEKPOINT_BEGIN, 0);
		if (_loopFlg) {
			for (uint32_t c = 0; c < m_Format.channnels; ++c) {
				std::memcpy(&_dest[c][sendFrames], &m_RealWave[c].front(), sizeof(float) * remainFrames);
			}
			m_Cursor = remainFrames;
		}
	}
	else {
		m_Cursor += sendFrames;
	}

	return sendFrames + remainFrames;
}

const uint32_t AS::WaveBase::Seek(const ESeekPoint _point, const uint32_t _seek) {
	if (_seek > m_AllFrames) {
		return 0;
	}
	//再帰ロック
	std::lock_guard<std::recursive_mutex> lock(m_WavMutex);

	switch (_point) {
	case ESeekPoint::WAVE_SEEKPOINT_BEGIN:
		m_Cursor = _seek;
		break;
	case ESeekPoint::WAVE_SEEKPOINT_END:
		m_Cursor = m_AllFrames - _seek;
		break;
	default:
		return 0;
	}
	if (m_BufMode == EBufferMode::WAVE_BUFFERMODE_STREAM) {
		SeekStream(m_Cursor);
	}
	return m_Cursor;
}