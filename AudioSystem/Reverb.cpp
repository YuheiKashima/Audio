#include "Reverb.h"

AS::Reverb::Reverb(AudioFormat _format) :EffectBase(_format) {
	m_Comb = std::make_shared<TEMPLATE::ParallelEffector<CombFilter, 4>>(_format);
	m_BiquadFilters.resize(m_Format.channnels);
}

AS::Reverb::~Reverb() {
}

void AS::Reverb::SetEffectParam(EffectParamBase& _param) {
	std::lock_guard lock(m_ParamMutex);
	m_Param = static_cast<ReverbParam&>(_param);

	for (uint32_t i = 0; i < m_Comb->GetParallelSize(); ++i) {
		auto comb = m_Comb->At(i).lock();
		comb->SetEffectParam(m_Param.comb[i]);
		comb->SetEnable(m_Param.combEnable[i]);
	}
	for (auto& biquad : m_BiquadFilters) {
		for (size_t i = 0; auto & filter : biquad) {
			filter.AllPass(m_Format.samplingRate, 1200, m_Param.apfQ[i++]);
		}
	}
	m_Param.wet = m_Param.wet > 0.0f ? m_Param.wet < 1.0f ? m_Param.wet : 1.0f : 0.0f;
	m_Param.dry = m_Param.dry > 0.0f ? m_Param.dry < 1.0f ? m_Param.dry : 1.0f : 0.0f;
}

void AS::Reverb::Process(LineBuffer<float>& _buffer, uint32_t _renderFrames) {
	std::lock_guard lock(m_ParamMutex);
	if (m_DestTempBuf.empty()) {
		m_DestTempBuf = _buffer;
		m_DestTempBuf.zeroclear();
	}

	//並列コムフィルタ
	m_DestTempBuf = _buffer;
	m_Comb->Process(m_DestTempBuf, _renderFrames);

	//直列オールパスフィルタ
	for (uint32_t chan = 0; chan < m_Format.channnels; ++chan) {
		auto& biquad = m_BiquadFilters.at(chan);
		for (auto& filter : biquad) {
			for (uint32_t fram = 0; fram < _renderFrames; ++fram)
				m_DestTempBuf[chan][fram] = filter.Process(m_DestTempBuf[chan][fram]);
		}
	}

	if (GetEnable()) {
		m_DestTempBuf.avx_mul(m_Param.wet);
		_buffer.avx_mul(m_Param.dry);
		_buffer.avx_add(m_DestTempBuf);
	}
}

void AS::Reverb::Flush() {
	m_Comb->Flush();
	for (auto& biquad : m_BiquadFilters)
		for (auto& filter : biquad)
			filter.Flush();
}