#ifndef _COMBFILTER_
#define _COMBFILTER_
#include "EffectBase.h"

namespace AS {
	enum class COMBFILTERTYPE {
		COMBTYPE_FEEDBACK,
		COMBTYPE_FEEDFORWARD
	};

	struct CombParam :public EffectParamBase {
		CombParam() {}
		CombParam(float _delayTime, float _decayLevel)
			:delayTime(_delayTime), decayLevel(_decayLevel) {}
		float delayTime = 1.0f;
		float decayLevel = 1.0f;
	};

	class CombFilter :public EffectBase {
	public:
		CombFilter(AudioFormat _format);
		~CombFilter();

		void SetEffectParam(EffectParamBase& _param) override;
		void Process(LineBuffer<float>& _buffer, int32_t _renderFrames) override;
		void Flush()override;
	private:

		CombParam m_Param;
		int32_t m_Delaylength = 0;

		std::vector<boost::circular_buffer<double>> m_DelayBuffer;
	};
};

#endif