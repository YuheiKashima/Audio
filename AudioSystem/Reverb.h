#ifndef _REVERB_
#define _REVERB_
#include "CombFilter.h"
#include "AllpassFilter.h"
#include "ParallelEffector.h"

namespace AS {
	struct ReverbParam :public EffectParamBase {
		ReverbParam() {}
		ReverbParam(
			std::array<CombParam, 4> _comb,
			std::array<AllpassParam, 2> _apf,
			std::array<bool, 4> _combEnable,
			std::array<bool, 2> _apfEnable,
			float _dry,
			float _wet
		) :combParams(_comb), apfParams(_apf), combEnable(_combEnable), apfEnable(_apfEnable), dry(_dry), wet(_wet) {}
		std::array<CombParam, 4> combParams{ CombParam(1.0f,1.0f),CombParam(1.0f,1.0f) ,CombParam(1.0f,1.0f),CombParam(1.0f,1.0f) };
		std::array<AllpassParam, 2>apfParams{ AllpassParam(1.0,1.0),AllpassParam(1.0,1.0) };
		std::array<bool, 4> combEnable{ true,true,true,true };
		std::array<bool, 2> apfEnable{ true,true };
		float dry = 1.0f, wet = 1.0f;
	};

	class Reverb :public EffectBase {
	public:
		Reverb(AudioFormat _format);
		~Reverb();

		void SetEffectParam(EffectParamBase& _param) override;
		void Process(LineBuffer<float>& _buffer, int32_t _renderFrames) override;
		void Flush() override;
		void SetEnable(const bool _enable)override {
			EffectBase::SetEnable(_enable);
			m_Comb.SetEnable(_enable);
		}
	private:
		TEMPLATE::ParallelEffector<CombFilter, 4> m_Comb;
		std::vector<std::array<std::shared_ptr<AllpassFilter>, 2>> m_APFs;

		ReverbParam m_Param;
		LineBuffer<float>  m_DestTempBuf;
	};
}
#endif