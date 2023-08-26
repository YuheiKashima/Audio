#include "Compressor.h"

AS::Compressor::Compressor(AudioFormat _format) :EffectBase(_format) {
}

AS::Compressor::~Compressor() {
}

void AS::Compressor::SetEffectParam(EffectParamBase& _param) {
	std::lock_guard lock(m_ParamMutex);
	m_Param = static_cast<CompressorParam&>(_param);

	for (auto& env : m_EnvFilter)
		env.LowPass(m_Format.samplingRate, 30.0, 1.0);
	for (auto& gain : m_GainFilter)
		gain.LowPass(m_Format.samplingRate, 30.0, 1.0);
}

void AS::Compressor::Process(LineBuffer<float>& _buffer, int32_t _renderFrames) {
	std::lock_guard lock(m_ParamMutex);
	LineBuffer<float> copy = _buffer;

	for (int32_t chan = 0; chan < m_Format.channnels; ++chan) {
		auto& env = m_EnvFilter.at(chan);
		auto& gain = m_GainFilter.at(chan);

		//���͒l�̐�Βl�����[�p�X�Ɋ|���ĉ��������m
		env.Process(&copy[chan].front(), _renderFrames);

		for (int32_t fram = 0; fram < _renderFrames; ++fram) {
			if (copy[chan][fram] > m_Param.threshold) {
				//臒l�𒴂���Ɖ���(�Q�C��)�𒲐�(���k)
				copy[chan][fram] = m_Param.threshold + (copy[chan][fram] - m_Param.threshold) / m_Param.ratio;
			}
		}

		//����(�Q�C��)���}���ɕω����Ȃ��悤���[�p�X��ʂ�
		gain.Process(&copy[chan].front(), _renderFrames);
	}

	copy.mul(m_Param.gain);

	//���͂ɉ���(�Q�C��)���|���A����ɍŏI�I�ȉ��ʂ𒲐߂��o��
	if (GetEnable()) {
		_buffer = copy;
	}
}

void AS::Compressor::Flush() {
	for (auto& env : m_EnvFilter)
		env.Flush();
	for (auto& gain : m_GainFilter)
		gain.Flush();
}