#ifndef _COMPRESSOR_
#define _COMPRESSOR_
#include "EffectBase.h"
#include "BiquadFilter.h"

namespace AS {
	struct CompressorParam :public EffectParamBase {
		CompressorParam() {}
		CompressorParam(float _threshold, float _ratio, float _gain) :threshold(_threshold), ratio(_ratio), gain(_gain) {}
		float threshold = 0.3f;
		float ratio = 2.0f;
		float gain = 2.0f;
	};

	class Compressor :public EffectBase {
	public:
		Compressor(AudioFormat _format);
		~Compressor();

		void SetEffectParam(EffectParamBase& _param)override;
		void Process(LineBuffer<float>& _buffer, uint32_t _renderFrames)override;
		void Flush()override;

	private:
		CompressorParam m_Param;

		std::array<BiquadFilter, 2> m_EnvFilter;
		std::array<BiquadFilter, 2> m_GainFilter;
	};
}
#endif