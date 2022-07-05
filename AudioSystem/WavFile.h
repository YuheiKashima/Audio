#ifndef _WAVFILE_
#define _WAVFILE_

#include "WaveBase.h"

namespace AS {
	struct RIFFChunk {
		char	chunk_ID[4]{ 'R','I','F','F' };		//!RIFFチャンク識別['R''I''F''F']
		long	chunk_size = 0;						//!IDとsizeを除くファイルサイズ
		char	file_type[4]{ 'W','A','V','E' };	//!音声データの場合['W''A''V''E']
	};

	struct FMTChunk {
		char	chunk_ID[4]{ 'f','m','t',' ' };		//!FMTチャンク識別['f''m''t'' ']
		long	chunk_size = 16;					//!IDとsizeを除くチャンクデータサイズ
		short	wave_format_type = 0x0001;			//!波形データのフォーマット	PCM:0x0001
		short	channels = 0;						//!使用チャンネル数	Mono:1	Ste:2
		long	samplingRate = 0;					//!サンプリング周波数
		long	bytes_per_sec = 0;					//!1sec当たりの音データ(block_size*Sampling_Frequency)
		short	block_size = 0;						//!1サンプル当たりの音データ(quantization*channel/8)
		short	bitDepth = 0;						//!量子化精度
	};

	struct DATAChunk {
		char	chunk_ID[4]{ 'd','a','t','a' };		//!DATAチャンク識別['d''a''t''a']
		long	chunk_size = 0;						//!IDとsizeを除くデータチャンクサイズ
	};

	//チャンク ジャンクデータ
	struct OtherChunk {
		char	chunk_ID[4];
		long	chunk_size = 0;
	};

	class WavFile : public WaveBase {
		template<typename T>
		friend class CStdPtrHelper;
	public:
		WavFile(std::string _directory, AudioFormat _createFormat) { Create(_directory, _createFormat); }
		WavFile(std::string _directory, EBufferMode _mode) { Open(_directory, _mode); };
		~WavFile();
		void Write(LineBuffer<float>& _writeBuf, const size_t _frames);
	private:
		enum class StreamMode {
			WAV_STREAMMODE_NONE,
			WAV_STREAMMODE_WRITE,
			WAV_STREAMMODE_LOAD
		};

		void Create(std::string _fileName, AudioFormat _createFormat);
		bool Open(std::string _directory, EBufferMode _mode);
		size_t GetStream(LineBuffer<float>& _dest, const uint32_t _frames, const bool _loopFlg, bool& _isEnd)override;
		void SeekStream(const uint32_t _seek) override;

		RIFFChunk m_RIFF;
		FMTChunk m_FMT;
		DATAChunk m_DATA;

		StreamMode m_StreamMode = StreamMode::WAV_STREAMMODE_NONE;
	};
}
#endif