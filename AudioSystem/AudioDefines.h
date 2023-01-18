#ifndef _AUDIODEFINES_
#define _AUDIODEFINES_

#include "Utilities.h"
#include "MLineBuffer.h"
#include "MCircularBuffer.h"
#include "MStdPtrHelper.h"
#include "MLog.h"
#include "MOTFFT.h"
#include "MCPUTimer.h"

namespace AS {
	using namespace myLib;
	using byte = uint8_t;
	const uint32_t BitsPerByte = 8;
	const int32_t NoPadding = -1;
	const float Volume_Min = 0.0f;
	const float Volume_Max = 2.0f;
	const uint32_t LoopInfinity = std::numeric_limits<uint32_t>::max BOOST_PREVENT_MACRO_SUBSTITUTION();
	const std::string DefaultDeviceStr = "(Default)";

	enum class TimerLayers {
		Timerlayer_None = 0x0000,
		Timerlayer_SystemTime = 0x0001,
		Timerlayer_MasterTime = 0x004,
		Timerlayer_SourceTime = 0x008,
		Timerlayer_IOTime = 0x0010,
		Timerlayer_RenderingTime = Timerlayer_SystemTime | Timerlayer_MasterTime | Timerlayer_SourceTime | Timerlayer_IOTime,

		//������`
		DTL_SyMaSo = Timerlayer_SystemTime | Timerlayer_MasterTime | Timerlayer_SourceTime,
		DTL_SySoIo = Timerlayer_SystemTime | Timerlayer_SourceTime | Timerlayer_IOTime,
		DTL_SyMa = Timerlayer_SystemTime | Timerlayer_MasterTime,
		DTL_SySo = Timerlayer_SystemTime | Timerlayer_SourceTime,
		DTL_MaSo = Timerlayer_MasterTime | Timerlayer_SourceTime,
		DTL_SoIo = Timerlayer_SourceTime | Timerlayer_IOTime
	};

	struct AudioFormat {
		AudioFormat() :samplingRate(0), bitDepth(0), channnels(0) {}
		AudioFormat(uint32_t _sampRate, uint32_t _bitDepth, uint32_t _channels) :samplingRate(_sampRate), bitDepth(_bitDepth), channnels(_channels) {
		}

		bool operator ==(const AudioFormat _comp) {
			if (samplingRate != _comp.samplingRate)return false;
			if (bitDepth != _comp.bitDepth)return false;
			if (channnels != _comp.channnels)return false;
			return true;
		}
		//�T���v�����O���g��
		uint32_t samplingRate;
		//�ʎq���r�b�g��
		uint32_t bitDepth;
		//�`�����l����
		uint32_t channnels;
	};

	class PCMNormalizer {
	public:
		static void PCM_Normalize(byte* _pSrc, LineBuffer<float>& _dest, const AudioFormat _format, const uint32_t _frames);
		static void PCM_Denormalize(LineBuffer<float>& _src, byte* _pDest, const AudioFormat _format, const uint32_t _frames);

	private:
		static void PCM_Normalize_8bit(byte* _pSrc, float* _pDest);
		static void PCM_Denormalize_8bit(float* _pSrc, byte* _pDest);
		static void PCM_Normalize_16bit(byte* _pSrc, float* _pDest);
		static void PCM_Denormalize_16bit(float* _pSrc, byte* _pDest);
		static void PCM_Normalize_32bit(byte* _pSrc, float* _pDest);
		static void PCM_Denormalize_32bit(float* _pSrc, byte* _pDest);
	};

	uint32_t TimeToFrames(const AudioFormat _format, const uint32_t _time);
	uint32_t FramesToTime(const AudioFormat _format, const uint32_t _frames);
}
#endif