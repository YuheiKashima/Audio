#include "SchroederReverb.h"

AS::SchroederReverb::SchroederReverb(AudioFormat _format) :EffectBase(_format), m_CombFilters{ _format, _format, _format, _format }, m_Equalizer(_format) {
}

AS::SchroederReverb::~SchroederReverb() {
}

void AS::SchroederReverb::SetEffectParam(EffectParamBase& _param) {
	for (uint32_t co = 0; auto & comb : m_CombFilters) {
		comb.SetEffectParam(static_cast<SchroederParam&>(_param).combParam[co]);
	}
	m_Equalizer.SetEffectParam(static_cast<SchroederParam&>(_param).eqParam);
}

void AS::SchroederReverb::Process(LineBuffer<float>& _buffer, uint32_t _renderFrames) {
}

void AS::SchroederReverb::Flush() {
	for (auto& comb : m_CombFilters) {
		comb.Flush();
	}
	m_Equalizer.Flush();
}