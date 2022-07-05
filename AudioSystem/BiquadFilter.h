#ifndef _BIQUADFILTER_
#define _BIQUADFILTER_
#include "EffectBase.h"

namespace AS {
	const uint32_t cBuffers = 2;
	const uint32_t cCoeficients = 3;
	class BiquadFilter {
	public:
		BiquadFilter();
		~BiquadFilter();

		float Process(float _src);
		void Flush();

		void LowPass(uint32_t _samplingFreq, float _cutoffFreq, float _q);
		void HighPass(uint32_t _samplingFreq, float _cutoffFreq, float _q);
		void BandPass(uint32_t _samplingFreq, float _centerFreq, float _bandwidth);
		void Notch(uint32_t _samplingFreq, float _centerFreq, float _bandwidth);
		void LowShelf(uint32_t _samplingFreq, float _cutoffFreq, float _q, float _gain);
		void HighShelf(uint32_t _samplingFreq, float _cutoffFreq, float _q, float _gain);
		void Peaking(uint32_t _samplingFreq, float _centerFreq, float _bandwidth, float _gain);
		void AllPass(uint32_t _samplingFreq, float _centerFreq, float _q);

	private:
		struct IIRBuffer {
			std::array<float, cBuffers> in{ 0.0f,0.0f };
			std::array<float, cBuffers> out{ 0.0f,0.0f };
			void Flush() { in = out = std::array<float, cBuffers>{0.0f, 0.0f}; }
		};
		struct BiquadCoeficients {
			BiquadCoeficients() {}
			BiquadCoeficients(float _alp0, float _alp1, float _alp2, float _bta0, float _bta1, float _bta2) {
				alpha = std::array<float, cCoeficients>{_alp0, _alp1, _alp2};
				beta = std::array<float, cCoeficients>{_bta0, _bta1, _bta2};
			}
			BiquadCoeficients(std::array<float, cCoeficients> _alpha, std::array<float, cCoeficients> _beta) :alpha(_alpha), beta(_beta) {
			}
			std::array<float, cCoeficients> alpha{ 0.0f,0.0f,0.0f };
			std::array<float, cCoeficients> beta{ 0.0f,0.0f,0.0f };
		};

		IIRBuffer m_IIRBuff;
		BiquadCoeficients m_BiCoef;
	};
}

#endif