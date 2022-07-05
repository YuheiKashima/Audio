#ifndef _WAVFILE_
#define _WAVFILE_

#include "WaveBase.h"

namespace AS {
	struct RIFFChunk {
		char	chunk_ID[4]{ 'R','I','F','F' };		//!RIFF�`�����N����['R''I''F''F']
		long	chunk_size = 0;						//!ID��size�������t�@�C���T�C�Y
		char	file_type[4]{ 'W','A','V','E' };	//!�����f�[�^�̏ꍇ['W''A''V''E']
	};

	struct FMTChunk {
		char	chunk_ID[4]{ 'f','m','t',' ' };		//!FMT�`�����N����['f''m''t'' ']
		long	chunk_size = 16;					//!ID��size�������`�����N�f�[�^�T�C�Y
		short	wave_format_type = 0x0001;			//!�g�`�f�[�^�̃t�H�[�}�b�g	PCM:0x0001
		short	channels = 0;						//!�g�p�`�����l����	Mono:1	Ste:2
		long	samplingRate = 0;					//!�T���v�����O���g��
		long	bytes_per_sec = 0;					//!1sec������̉��f�[�^(block_size*Sampling_Frequency)
		short	block_size = 0;						//!1�T���v��������̉��f�[�^(quantization*channel/8)
		short	bitDepth = 0;						//!�ʎq�����x
	};

	struct DATAChunk {
		char	chunk_ID[4]{ 'd','a','t','a' };		//!DATA�`�����N����['d''a''t''a']
		long	chunk_size = 0;						//!ID��size�������f�[�^�`�����N�T�C�Y
	};

	//�`�����N �W�����N�f�[�^
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