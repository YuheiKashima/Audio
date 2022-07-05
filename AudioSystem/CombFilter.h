#ifndef _COMBFILTER_
#define _COMBFILTER_
#include "EffectBase.h"

namespace AS {
	struct CombParam :public EffectParamBase {
		CombParam() {}
		CombParam(float _delayTime, float _decayLevel, float _feedBack)
			:delayTime(_delayTime), decayLevel(_decayLevel), feedBack(_feedBack) {}
		CombParam(float _delayTime, float _feedBackDecay)
			:delayTime(_delayTime), decayLevel(_feedBackDecay), feedBack(_feedBackDecay) {}

		float delayTime = 0.0f;
		float decayLevel = 0.0f;
		float feedBack = 0.0f;
	};

	class CombFilter :public EffectBase {
	public:
		CombFilter(AudioFormat _format);
		~CombFilter();

		void SetEffectParam(EffectParamBase& _param) override;
		void Process(LineBuffer<float>& _buffer, uint32_t _renderFrames) override;
		void Flush()override;
	private:

		CombParam m_Param;
		uint32_t m_DelayPoint = 0;
		std::vector<CircularBuffer<float>> m_CircularBuffers;
	};
};

#endif