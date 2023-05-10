#include "Equalizer.h"

AS::Equalizer::Equalizer(AudioFormat _format) :EffectBase(_format) {
	m_IIRFilters.resize(m_Format.channnels);
}

AS::Equalizer::~Equalizer() {
}

void AS::Equalizer::SetEffectParam(EffectParamBase& _param) {
	std::lock_guard lock(m_ParamMutex);
	m_Param = static_cast<EQParam&>(_param);
	for (auto& biquad : m_IIRFilters) {
		for (uint32_t i = 0; auto & filter : biquad) {
			//ピーキングフィルタをセット
			filter.Peaking(m_Format.samplingRate, m_EQFreqs[i], m_Param.bandwidth, m_Param.eqGain[i]);
			++i;
		}
	}
}

void AS::Equalizer::Process(LineBuffer<float>& _buffer, uint32_t _renderFrames) {
	std::lock_guard lock(m_ParamMutex);
	LineBuffer<float> copy = _buffer;

	_buffer.mul(m_Param.preGain);
	for (uint16_t chan = 0; chan < m_Format.channnels; ++chan) {
		auto& biquad = m_IIRFilters.at(chan);
		float dest = 0.0f;
		for (auto& filter : biquad) {
			filter.Process(&copy[chan].front(), _renderFrames);
		}
	}
	if (GetEnable())
		_buffer = copy;
}

void AS::Equalizer::Flush() {
	for (auto& biquad : m_IIRFilters)
		for (auto& filter : biquad)
			filter.Flush();
}