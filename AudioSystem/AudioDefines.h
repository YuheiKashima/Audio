#ifndef _AUDIODEFINES_
#define _AUDIODEFINES_

#include "Utilities.h"
#include "MLineBuffer.h"
#include "MCircularBuffer.h"
#include "MStdPtrHelper.h"
#include "MLog.h"
#include "MOTFFT.h"

#define CHRONO_CONVERSION 10000.0
#define MEASUREMENT_AVERAGE 100
#define MEASUREMENT_MASTER false
#define MEASUREMENT_RENDER true

namespace AS {
	using namespace myLib;
	using byte = uint8_t;
	const uint32_t BitsPerByte = 8;
	const int32_t NoPadding = -1;
	const float Volume_Min = 0.0f;
	const float Volume_Max = 2.0f;
	const uint32_t LoopInfinity = std::numeric_limits<uint32_t>::max();
	const std::string DefaultDeviceStr = "(Default)";

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

		uint32_t samplingRate;
		uint32_t bitDepth;
		uint32_t channnels;
	};

	struct Track {
		LineBuffer<float> buffer;
		std::shared_mutex mutex;
		size_t fillingBuffer = 0;
		bool is_End = false;
	};

	class PCMNormalizer {
	public:
		static void PCM_Normalize(byte* _pSrc, LineBuffer<float>& _dest, const AudioFormat _format, const uint32_t _frames);
		static void PCM_Denormalize(LineBuffer<float>& _src, byte* _pDest, const AudioFormat _format, const uint32_t _frames);

	private:
		static void PCM_Normalize_16bit(byte* _pSrc, float* _pDest);
		static void PCM_Denormalize_16bit(float* _pSrc, byte* _pDest);
	};

	uint32_t TimeToFrames(const AudioFormat _format, const uint32_t _time);
	uint32_t FramesToTime(const AudioFormat _format, const uint32_t _frames);
}
#endif