#ifndef _REVERB_
#define _REVERB_
#include "CombFilter.h"
#include "BiquadFilter.h"
#include "ParallelEffector.h"

namespace AS {
	struct ReverbParam :public EffectParamBase {
		ReverbParam() {}
		ReverbParam(std::array<CombParam, 4> _comb, std::array<bool, 4> _combEnable, std::array<float, 2> _apfQ, std::array<bool, 2> _apfEnable, float _dry, float _wet) :comb(_comb), combEnable(_combEnable), apfQ(_apfQ), apfEnable(_apfEnable), dry(_dry), wet(_wet) {}
		std::array<CombParam, 4> comb{ CombParam(1.0f,1.0f),CombParam(1.0f,1.0f) ,CombParam(1.0f,1.0f),CombParam(1.0f,1.0f) };
		std::array<bool, 4> combEnable{ true,true,true,true };
		std::array<float, 2> apfQ{ 1.0f,1.0f };
		std::array<bool, 2> apfEnable{ true,true };
		float dry = 1.0f, wet = 1.0f;
	};

	class Reverb :public EffectBase {
	public:
		Reverb(AudioFormat _format);
		~Reverb();

		void SetEffectParam(EffectParamBase& _param) override;
		void Process(LineBuffer<float>& _buffer, uint32_t _renderFrames) override;
		void Flush() override;
		void SetEnable(const bool _enable)override {
			EffectBase::SetEnable(_enable);
			m_Comb->SetEnable(_enable);
		}
	private:
		std::shared_ptr<TEMPLATE::ParallelEffector<CombFilter, 4>> m_Comb;
		std::vector<std::array<BiquadFilter, 2>> m_BiquadFilters;

		ReverbParam m_Param;
		LineBuffer<float>  m_DestTempBuf;
	};
}
#endif