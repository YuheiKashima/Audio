#include "Reverb.h"

AS::Reverb::Reverb(AudioFormat _format) :EffectBase(_format), m_Comb(_format) {
	for (int32_t i = 0; i < m_Format.channnels; ++i) {
		m_APFs.push_back({ std::make_shared<AllpassFilter>(m_Format), std::make_shared<AllpassFilter>(m_Format) });
	}
}

AS::Reverb::~Reverb() {
}

void AS::Reverb::SetEffectParam(EffectParamBase& _param) {
	std::lock_guard lock(m_ParamMutex);
	m_Param = static_cast<ReverbParam&>(_param);

	for (int32_t i = 0; i < m_Comb.GetParallelSize(); ++i) {
		auto comb = m_Comb.At(i).lock();
		comb->SetEffectParam(m_Param.combParams[i]);
		comb->SetEnable(m_Param.combEnable[i]);
	}
	for (auto& apf : m_APFs) {
		for (size_t i = 0; auto & filter : apf) {
			filter->SetEffectParam(m_Param.apfParams[i]);
			++i;
		}
	}
	m_Param.wet = m_Param.wet > 0.0f ? m_Param.wet < 1.0f ? m_Param.wet : 1.0f : 0.0f;
	m_Param.dry = m_Param.dry > 0.0f ? m_Param.dry < 1.0f ? m_Param.dry : 1.0f : 0.0f;
}

void AS::Reverb::Process(LineBuffer<float>& _buffer, int32_t _renderFrames) {
	std::lock_guard lock(m_ParamMutex);

	//並列コムフィルタ
	m_DestTempBuf = _buffer;
	m_Comb.Process(m_DestTempBuf, _renderFrames);

	//直列オールパスフィルタ
	for (int32_t chan = 0; auto & apfs:m_APFs) {
		for (auto& filter : apfs) {
			filter->Process(m_DestTempBuf, _renderFrames);
		}
		++chan;
	}

	if (GetEnable()) {
		m_DestTempBuf.mul(m_Param.wet);
		_buffer.mul(m_Param.dry);
		_buffer.add(m_DestTempBuf);
	}
}

void AS::Reverb::Flush() {
	m_Comb.Flush();
	for (auto& biquad : m_APFs)
		for (auto& filter : biquad)
			filter->Flush();
	m_DestTempBuf.Reset();
}