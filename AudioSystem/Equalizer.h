#ifndef _EQUALIZER_
#define _EQUALIZER_

#include "EffectBase.h"
#include "IIRFilter.h"

//方針
//イコライザはピーキングのみで
//foobarのグラフィックイコライザの周波数を使用)55,77,110,156,220,311,440,622,880,1.2k,1.8k,2.5k,3.5k,5k,7k,10k,14k,20k,

namespace AS {
	struct EQParam :public EffectParamBase {
		EQParam() {}
		EQParam(double _preGain,
			double _55Hz, double _77Hz, double _110Hz, double _156Hz, double _220Hz, double _311Hz, double _440Hz, double _622Hz, double _880Hz, double _1_2kHz, double _1_8kHz, double _2_5kHz, double _3_5kHz, double _5kHz, double _7kHz, double _10kHz, double _14kHz, double _20kHz,
			double _bandWidth) :preGain(_preGain),
			eqGain{ _55Hz, _77Hz, _110Hz, _156Hz, _220Hz, _311Hz, _440Hz, _622Hz, _880Hz, _1_2kHz, _1_8kHz, _2_5kHz, _3_5kHz, _5kHz, _7kHz, _10kHz, _14kHz, _20kHz },
			bandwidth(_bandWidth) {}
		double preGain = 1.0f;
		std::array<double, 18> eqGain{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
		double bandwidth = 1.0f;
	};

	class Equalizer : public EffectBase {
	public:
		Equalizer(AudioFormat _format);
		~Equalizer();

		void SetEffectParam(EffectParamBase& _param) override;
		void Process(LineBuffer<float>& _buffer, int32_t _renderFrames) override;
		void Flush()override;

	private:
		std::array<const double, 18>m_EQFreqs{ 55.0f,77.0f,110.0f,156.0f,220.0f,311.0f,440.0f,622.0f,880.0f,1200.0f,1800.0f,2500.0f,3500.0f,5000.0f,7000.0f,10000.0f,14000.0f,20000.0f };
		std::vector<std::array<IIRFilter, 18>> m_IIRFilters;
		EQParam m_Param;

		LineBuffer<float> m_DestTempBuf;
	};
}

#endif