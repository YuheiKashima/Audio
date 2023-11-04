#ifndef _ALLPASSFILTER_
#define _ALLPASSFILTER_
#include "EffectBase.h"

namespace AS {
	struct AllpassParam :public EffectParamBase {
		AllpassParam() {}
		AllpassParam(double _delayTime, double _decayLevel)
			:delayTime(_delayTime), decayLevel(_decayLevel) {
		}

		double delayTime = 1.0;
		double decayLevel = 1.0;
	};

	class AllpassFilter :public EffectBase {
	public:
		AllpassFilter(AudioFormat _format);
		~AllpassFilter();

		void SetEffectParam(EffectParamBase& _param)override;
		void Process(LineBuffer<float>& _buffer, int32_t _renderFrames)override;
		void Flush()override;
	private:
		AllpassParam m_Param;
		int32_t m_DelayLength = 0;

		std::vector<boost::circular_buffer<double>> m_DelayBuffer;
	};
}

#endif
