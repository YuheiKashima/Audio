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
		for (int32_t i = 0; auto & filter : biquad) {
			//ピーキングフィルタをセット
			filter.Peaking(m_Format.samplingRate, m_EQFreqs[i], m_Param.bandwidth, m_Param.eqGain[i]);
			++i;
		}
	}
}

void AS::Equalizer::Process(LineBuffer<float>& _buffer, int32_t _renderFrames) {
	std::lock_guard lock(m_ParamMutex);
	m_DestTempBuf = _buffer;

	_buffer.mul(m_Param.preGain);
	for (auto chan = 0; chan < m_Format.channnels; ++chan) {
		auto& biquad = m_IIRFilters.at(chan);
		float dest = 0.0f;
		for (auto& filter : biquad) {
			filter.Process(&m_DestTempBuf.at(chan).front(), _renderFrames);
		}
	}
	if (GetEnable())
		_buffer = m_DestTempBuf;
}

void AS::Equalizer::Flush() {
	for (auto& biquad : m_IIRFilters)
		for (auto& filter : biquad)
			filter.Flush();
	m_DestTempBuf.Reset();
}