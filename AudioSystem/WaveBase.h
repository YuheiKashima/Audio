#ifndef _WAVEBASE_
#define _WAVEBASE_
#include "AudioDefines.h"

namespace AS {
	enum class EBufferMode {
		WAVE_BUFFERMODE_NONE = 0,
		WAVE_BUFFERMODE_LOADALL,
		WAVE_BUFFERMODE_STREAM
	};

	enum class ESeekPoint {
		WAVE_SEEKPOINT_BEGIN,
		WAVE_SEEKPOINT_END
	};

	class WaveBase {
	public:
		WaveBase();
		virtual ~WaveBase();

		size_t GetBuffer(LineBuffer<float>& _dest, const uint32_t _frames, const bool _loopFlg, bool& _isEnd);
		const uint32_t Seek(const ESeekPoint _point, const uint32_t _seek);
		const LineBuffer<float>& GetTrack() { return m_RealWave; }
		const AudioFormat GetFormat() { return m_Format; }
		const uint32_t Size() { return m_AllFrames; }
	protected:
		virtual size_t GetStream(LineBuffer<float>& _dest, const uint32_t _frames, const bool _loopFlg, bool& _isEnd) = 0;
		size_t GetMemory(LineBuffer<float>& _dest, const uint32_t _frames, const bool _loopFlg, bool& _isEnd);

		virtual void SeekStream(const uint32_t _seek) = 0;

		void OutputLoadLog(std::string _fileType, std::string _directory, EBufferMode _bufMode, AudioFormat _format, const uint32_t _allFrames) {
			std::string mode;
			switch (_bufMode) {
			case AS::EBufferMode::WAVE_BUFFERMODE_LOADALL:
				mode += "LoadAll";
				break;
			case AS::EBufferMode::WAVE_BUFFERMODE_STREAM:
				mode += "Stream";
				break;
			default:
				mode += "None";
				break;
			}

			std::stringstream strstr;
			strstr << "FileType\t:" << _fileType << std::endl;
			strstr << "Directory\t:" << _directory << std::endl;
			strstr << "BufferMode\t:" << mode << std::endl;
			strstr << "TrackFormat<" << std::endl;
			strstr << "\t" << "Channel\t\t:" << _format.channnels << std::endl;
			strstr << "\t" << "SamplingRate\t:" << _format.samplingRate << std::endl;
			strstr << "\t" << "BitDepth\t:" << _format.bitDepth << std::endl;
			strstr << ">" << std::endl;
			strstr << "AllFrameSize\t:" << _allFrames << "(" << FramesToTime(_format, _allFrames) << "ms)" << std::endl;

			Log::Logging(Log::ASLOG_INFO, strstr.str(), std::source_location::current(), true);
		}

		AudioFormat m_Format{};
		uint32_t m_AllFrames;
		EBufferMode m_BufMode = EBufferMode::WAVE_BUFFERMODE_NONE;

		LineBuffer<float> m_RealWave;
		uint32_t m_Cursor = 0;

		std::fstream m_WaveStream;
		uint32_t m_StreamBeginPoint = 0;

		std::recursive_mutex m_WavMutex;

		const std::string m_cOutputDirectory = "Media";
		const std::string m_cTempDirectory = "Temp";
		std::string m_FileDirectory;
		std::string m_TempDirectory;
	};
}

#endif