#include "WavFile.h"

AS::WavFile::~WavFile() {
	if (m_StreamMode == StreamMode::WAV_STREAMMODE_WRITE) {
		DATAChunk data;
		data.chunk_size = m_WaveStream.tellg();
		RIFFChunk riff;
		riff.chunk_size = sizeof(DATAChunk) + sizeof(FMTChunk) + data.chunk_size;
		if (m_WaveStream.is_open())m_WaveStream.close();

		std::ofstream output(m_FileDirectory, std::ios::binary);
		if (!output.is_open())return;
		std::ifstream temp(m_TempDirectory, std::ios::binary);
		if (!temp.is_open())return;

		output.write((char*)&riff, sizeof(RIFFChunk));
		output.write((char*)&m_FMT, sizeof(FMTChunk));
		output.write((char*)&data, sizeof(DATAChunk));

		std::unique_ptr<byte[]> buf(new byte[data.chunk_size / sizeof(byte)]);
		temp.read((char*)buf.get(), data.chunk_size);
		output.write((char*)buf.get(), data.chunk_size);
	}

	if (m_WaveStream.is_open()) {
		m_WaveStream.close();
	}
}

void AS::WavFile::Create(std::string _fileName, AudioFormat _createFormat) {
	if (m_StreamMode != StreamMode::WAV_STREAMMODE_NONE)return;

	std::filesystem::create_directory("../" + m_cOutputDirectory);
	std::filesystem::create_directory("../" + m_cTempDirectory);
	m_FileDirectory = "../" + m_cOutputDirectory + "/" + _fileName + ".wav";
	m_TempDirectory = "../" + m_cTempDirectory + "/" + Chrono::GetTime_str() + ".tmp";

	FMTChunk fmt;
	fmt.channels = _createFormat.channnels;
	fmt.bitDepth = _createFormat.bitDepth;
	fmt.samplingRate = _createFormat.samplingRate;
	fmt.block_size = (fmt.bitDepth / BitsPerByte) * fmt.channels;
	fmt.bytes_per_sec = fmt.samplingRate * fmt.block_size;

	std::fstream tempStr(m_TempDirectory, std::ios_base::out | std::ios_base::binary);
	if (!tempStr.is_open()) return;

	m_FMT = fmt;
	m_WaveStream = std::move(tempStr);
	m_StreamMode = StreamMode::WAV_STREAMMODE_WRITE;
	return;
}

void AS::WavFile::Write(LineBuffer<float>& _writeBuf, const size_t _frames) {
	if (m_StreamMode != StreamMode::WAV_STREAMMODE_WRITE || !m_WaveStream.is_open())return;
	uint32_t size = (m_FMT.bitDepth / BitsPerByte) * m_FMT.channels * _frames;
	std::unique_ptr<byte[]> buf(new byte[size]);

	memset(buf.get(), NULL, sizeof(byte) * size);

	PCMNormalizer::PCM_Denormalize(
		_writeBuf,
		buf.get(),
		AudioFormat(m_FMT.samplingRate, m_FMT.bitDepth, m_FMT.channels),
		_frames);

	m_WaveStream.write((char*)buf.get(), sizeof(byte) * size);
}

bool AS::WavFile::Open(std::string _directory, EBufferMode _mode) {
	if (m_StreamMode != StreamMode::WAV_STREAMMODE_NONE)return false;

	m_FileDirectory = _directory;
	std::fstream fstr(m_FileDirectory, std::ios::in | std::ios::binary);
	if (!fstr.is_open()) return false;

	uint32_t wavPoint = 0;
	{
		char id[5];
		while (!fstr.eof()) {
			memset(id, NULL, 5);
			fstr.read(id, 4);
			fstr.seekg(wavPoint);
			std::string str_id(id);
			if (str_id.compare("RIFF") == 0) {
				fstr.read((char*)&m_RIFF, sizeof(RIFFChunk));
				wavPoint = (uint32_t)fstr.tellg();
			}
			else if (str_id.compare("fmt ") == 0) {
				fstr.read((char*)&m_FMT, sizeof(FMTChunk));
				wavPoint = (uint32_t)fstr.tellg();
			}
			else if (str_id.compare("data") == 0) {
				fstr.read((char*)&m_DATA, sizeof(DATAChunk));
				wavPoint = (uint32_t)fstr.tellg();
				break;
			}
			else if (str_id.compare("junk") == 0) {
				OtherChunk junk{};
				fstr.read((char*)&junk, sizeof(OtherChunk));
				std::unique_ptr<byte[]> junkData(new byte[junk.chunk_size]);
				fstr.read((char*)junkData.get(), junk.chunk_size);
				wavPoint = (uint32_t)fstr.tellg();
			}
			else if (str_id.compare("LIST") == 0) {
				OtherChunk list{};
				fstr.read((char*)&list, sizeof(OtherChunk));
				std::unique_ptr<byte[]> listData(new byte[list.chunk_size]);
				fstr.read((char*)listData.get(), list.chunk_size);
				wavPoint = (uint32_t)fstr.tellg();
			}
			else {
				fstr.close();
				return false;
			}
		}
	}

	m_Format = AudioFormat(m_FMT.samplingRate, m_FMT.bitDepth, m_FMT.channels);
	m_AllFrames = m_DATA.chunk_size / m_FMT.channels / (m_FMT.bitDepth / BitsPerByte);
	m_BufMode = _mode;

	switch (m_BufMode) {
	case EBufferMode::WAVE_BUFFERMODE_LOADALL: {
		std::unique_ptr<byte[]>pcmData(new byte[m_DATA.chunk_size]);

		fstr.read((char*)pcmData.get(), m_DATA.chunk_size);
		fstr.close();

		m_RealWave.resize(m_Format.channnels, m_AllFrames);

		PCMNormalizer::PCM_Normalize(pcmData.get(), m_RealWave, m_Format, m_AllFrames);
		break;
	}
	case EBufferMode::WAVE_BUFFERMODE_STREAM: {
		m_StreamBeginPoint = wavPoint;
		m_WaveStream = std::move(fstr);
		break;
	}
	default:
		return false;
		break;
	}

	OutputLoadLog("wav", _directory, m_BufMode, m_Format, m_AllFrames);
	m_StreamMode = StreamMode::WAV_STREAMMODE_LOAD;

	return true;
}

size_t AS::WavFile::GetStream(LineBuffer<float>& _dest, const uint32_t _frames, const bool _loopFlg, bool& _isEnd) {
	if (!m_WaveStream.is_open() || m_StreamMode != StreamMode::WAV_STREAMMODE_LOAD)return 0;

	auto framesToFilePoint = m_Format.channnels * (m_Format.bitDepth / BitsPerByte);

	auto sendFrames = 0, remainFrames = 0;
	auto streamSize = 0, remainSize = 0;
	if ((m_Cursor + _frames) < m_AllFrames) {
		sendFrames = _frames;
		_isEnd = false;
	}
	else {
		sendFrames = m_AllFrames - m_Cursor;
		remainFrames = _loopFlg ? _frames - sendFrames : 0;
		_isEnd = true;
	}

	streamSize = sendFrames * framesToFilePoint;
	remainSize = remainFrames * framesToFilePoint;

	std::unique_ptr<byte[]> pcmData(new byte[streamSize + remainSize]);
	m_WaveStream.read((char*)pcmData.get(), streamSize);

	if (_isEnd) {
		//???????????????????????????????V?[?N?B
		Seek(ESeekPoint::WAVE_SEEKPOINT_BEGIN, 0);
		if (_loopFlg) {
			auto pCopy = (char*)pcmData.get() + streamSize;
			m_WaveStream.read(pCopy, remainSize);
			m_Cursor = remainFrames;
		}
	}
	else {
		m_Cursor += sendFrames;
	}

	PCMNormalizer::PCM_Normalize(pcmData.get(), _dest, m_Format, sendFrames + remainFrames);
	return sendFrames + remainFrames;
}

void AS::WavFile::SeekStream(const uint32_t _seek) {
	if (!m_WaveStream.is_open() || m_StreamMode != StreamMode::WAV_STREAMMODE_LOAD)return;

	uint32_t seekPoint = m_StreamBeginPoint + (_seek * m_Format.channnels * (m_Format.bitDepth / BitsPerByte));
	if (m_WaveStream.tellg() != seekPoint)
		m_WaveStream.seekg(seekPoint, std::ios::beg);
}