#ifndef _RESAMPLER_
#define _RESAMPLER_

#include "FIRFilter.h"

namespace AS {
	class Resampler {
	public:
		Resampler();
		~Resampler();
		static uint32_t CalcSamples(uint32_t _freqFrom, uint32_t _cntSammp, uint32_t _freqTo);
		uint32_t Resampling(LineBuffer<float>& _src, uint32_t _freqFrom, uint32_t _freqTo, LineBuffer<float>& _dest);
	private:
		FIRFilter m_lpf;

		uint32_t UpSampling(LineBuffer<float>& _src, double _resampRate, LineBuffer<float>& _dest);
		uint32_t DownSampling(LineBuffer<float>& _src, double _resampRate, LineBuffer<float>& _dest);
	};
}
#endif