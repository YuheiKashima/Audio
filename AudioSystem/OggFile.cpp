#include "OggFile.h"

AS::OggFile::OggFile(std::string _directory, EBufferMode _mode) {
	Open(_directory, _mode);
}

AS::OggFile::~OggFile() {
	ov_clear(&m_OggFile);
	m_OggFile = OggVorbis_File();
}

bool AS::OggFile::Open(std::string _directory, EBufferMode _mode) {
	int res = ov_fopen(_directory.c_str(), &m_OggFile);
	if (res != 0) {
		switch (res) {
		case OV_EREAD:       break;
		case OV_ENOTVORBIS:  break;
		case OV_EVERSION:    break;
		case OV_EBADHEADER:  break;
		case OV_EFAULT:      break;
		default:             break;
		}
		return false;
	}

	vorbis_info* pOggInfo = ov_info(&m_OggFile, -1);
	m_Format = AudioFormat(pOggInfo->rate, (pOggInfo->bitrate_nominal / pOggInfo->rate / pOggInfo->channels) * BitsPerByte, pOggInfo->channels);
	m_AllFrames = static_cast<uint32_t>(ov_time_total(&m_OggFile, -1) * m_Format.samplingRate);
	uint32_t pcmSize = m_AllFrames * m_Format.channnels * (m_Format.bitDepth / BitsPerByte);
	m_BufMode = _mode;

	switch (m_BufMode) {
	case AS::EBufferMode::WAVE_BUFFERMODE_LOADALL: {
		std::unique_ptr<byte[]> pcmData(new byte[pcmSize]);
		m_RealWave.resize(m_Format.channnels, m_AllFrames);

		byte* pPCM = pcmData.get();
		while (true) {
			int32_t dest = 0;
			int32_t bitStream = 0;
			dest = ov_read(&m_OggFile, reinterpret_cast<char*>(pPCM), OGG_REFREADSIZE, 0, 2, 1, &bitStream);
			if (dest == 0)break;

			pPCM += dest;
		}

		PCMNormalizer::PCM_Normalize(pcmData.get(), m_RealWave, m_Format, m_AllFrames);
		break;
	}
	case AS::EBufferMode::WAVE_BUFFERMODE_STREAM: {
		assert(ov_time_seek(&m_OggFile, 0.0) == 0);
		break;
	}
	default:
		return false;
		break;
	}

	OutputLoadLog("ogg", _directory, m_BufMode, m_Format, m_AllFrames);
	return true;
}

size_t AS::OggFile::GetStream(LineBuffer<float>& _dest, const uint32_t _frames, const bool _loopFlg, bool& _isEnd) {
	if (!is_Open(m_OggFile))return 0;

	const uint32_t orderSize = _frames * m_Format.channnels * (m_Format.bitDepth / BitsPerByte);
	int32_t requestSize = 0;
	int32_t bitStream = 0;
	int32_t readDestSize = 0;
	int32_t readCompleteSize = 0;
	std::unique_ptr<byte[]> pcmData(new byte[orderSize]);
	_isEnd = false;

	std::memset(pcmData.get(), NULL, sizeof(byte) * orderSize);
	byte* pPCM = pcmData.get();
	while (true) {
		requestSize = orderSize - readCompleteSize < OGG_REFREADSIZE ? orderSize - readCompleteSize : OGG_REFREADSIZE;
		readDestSize = 0;

		readDestSize = ov_read(&m_OggFile, reinterpret_cast<char*>(pPCM), requestSize, 0, 2, 1, &bitStream);

		//detect eof
		if (readDestSize <= 0) {
			assert(ov_pcm_seek(&m_OggFile, 0) == 0);
			if (_loopFlg) {
				m_Cursor = 0;
			}
			else {
				_isEnd = true;
				break;
			}
		}
		else if (readDestSize > 0) {
			readCompleteSize += readDestSize;
		}

		//バッファ残量判定
		if (readCompleteSize >= orderSize)
			break;
		else
			pPCM += readDestSize;
	}

	if (readCompleteSize > 0) {
		m_Cursor += readCompleteSize / m_Format.channnels * (m_Format.bitDepth / BitsPerByte);
		PCMNormalizer::PCM_Normalize(pcmData.get(), _dest, m_Format, _frames);
	}

	return readCompleteSize;
}

void AS::OggFile::SeekStream(const uint32_t _seek) {
	if (!is_Open(m_OggFile))return;
	double seek = _seek * (m_Format.samplingRate / 1000.0);

	if (seek <= m_AllFrames) {
		ogg_int64_t seekPoint = std::lround(seek * m_Format.channnels * (m_Format.bitDepth / BitsPerByte));
		ov_pcm_seek(&m_OggFile, seekPoint);
	}
}