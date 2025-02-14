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

		size_t GetBuffer(LineBuffer<float>& _dest, const int32_t _frames, const bool _loopFlg, bool& _isEnd);
		const int32_t Seek(const ESeekPoint _point, const int32_t _seek);
		const LineBuffer<float>& GetTrack() { return m_RealWave; }
		const AudioFormat GetFormat() { return m_Format; }
		const int32_t Size() { return m_AllFrames; }
		const int32_t GetFrameCount() { return m_Cursor; }
	protected:
		virtual size_t GetStream(LineBuffer<float>& _dest, const int32_t _frames, const bool _loopFlg, bool& _isEnd) = 0;
		virtual size_t GetMemory(LineBuffer<float>& _dest, const int32_t _frames, const bool _loopFlg, bool& _isEnd);

		virtual void SeekStream(const int32_t _seek) = 0;

		void OutputLoadLog(std::string _fileType, std::string _directory, EBufferMode _bufMode, AudioFormat _format, const int32_t _allFrames) {
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
			strstr << "\t" << "Channel\t:" << _format.channnels << std::endl;
			strstr << "\t" << "SamplingRate\t:" << _format.samplingRate << std::endl;
			strstr << "\t" << "BitDepth\t:" << _format.bitDepth << std::endl;
			strstr << ">" << std::endl;
			strstr << "AllFrameSize\t:" << _allFrames << "(" << FramesToTime(_format, _allFrames) << "ms)" << std::endl;

			Log::Logging(Log::ELoggingLevel::LOGLV_INFO, "{}", strstr.str());
		}

		AudioFormat m_Format{};
		//１チャンネルあたりの最大フレーム数
		int32_t m_AllFrames;
		EBufferMode m_BufMode = EBufferMode::WAVE_BUFFERMODE_NONE;

		LineBuffer<float> m_RealWave;
		int32_t m_Cursor = 0;

		std::fstream m_WaveStream;
		int32_t m_StreamBeginPoint = 0;

		std::recursive_mutex m_WavMutex;

		const std::string m_cOutputDirectory = "Media";
		const std::string m_cTempDirectory = "Temp";
		std::string m_FileDirectory;
		std::string m_TempDirectory;
	};
}

#endif