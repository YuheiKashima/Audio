#ifndef _EQUALIZER_
#define _EQUALIZER_

#include "BiquadFilter.h"

//方針
//イコライザはピーキングのみで
//バンド数をある程度固定 foobar(55,77,110,156,220,311,440,622,880,1.2k,1.8k,2.5k,3.5k,5k,7k,10k,14k,20k,

namespace AS {
	struct EQParam :public EffectParamBase {
		EQParam() {}
		EQParam(float _preGain,
			float _55Hz, float _77Hz, float _110Hz, float _156Hz, float _220Hz, float _311Hz, float _440Hz, float _622Hz, float _880Hz, float _1_2kHz, float _1_8kHz, float _2_5kHz, float _3_5kHz, float _5kHz, float _7kHz, float _10kHz, float _14kHz, float _20kHz,
			float _bandWidth) :preGain(_preGain),
			eqGain{ _55Hz, _77Hz, _110Hz, _156Hz, _220Hz, _311Hz, _440Hz, _622Hz, _880Hz, _1_2kHz, _1_8kHz, _2_5kHz, _3_5kHz, _5kHz, _7kHz, _10kHz, _14kHz, _20kHz },
			bandwidth(_bandWidth) {}
		float preGain = 1.0f;
		std::array<float, 18> eqGain{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
		float bandwidth = 1.0f;
	};

	class Equalizer : public EffectBase {
	public:
		Equalizer(AudioFormat _format);
		~Equalizer();

		void SetEffectParam(EffectParamBase& _param) override;
		void Process(LineBuffer<float>& _buffer, uint32_t _renderFrames) override;
		void Flush()override;

	private:
		std::array<const float, 18>m_EQFreqs{ 55.0f,77.0f,110.0f,156.0f,220.0f,311.0f,440.0f,622.0f,880.0f,1200.0f,1800.0f,2500.0f,3500.0f,5000.0f,7000.0f,10000.0f,14000.0f,20000.0f };
		std::vector<std::array<BiquadFilter, 18>> m_BiquadFilters;
		EQParam m_Param;
	};
}

#endif