#include "Compressor.h"

AS::Compressor::Compressor(AudioFormat _format) :EffectBase(_format) {
}

AS::Compressor::~Compressor() {
}

void AS::Compressor::SetEffectParam(EffectParamBase& _param) {
	std::lock_guard lock(m_ParamMutex);
	m_Param = static_cast<CompressorParam&>(_param);

	for (auto& env : m_EnvFilter)
		env.LowPass(m_Format.samplingRate, 30.0f, 1.0f);
	for (auto& gain : m_GainFilter)
		gain.LowPass(m_Format.samplingRate, 30.0f, 1.0f);
}

void AS::Compressor::Process(LineBuffer<float>& _buffer, uint32_t _renderFrames) {
	std::lock_guard lock(m_ParamMutex);

	for (uint32_t chan = 0; chan < m_Format.channnels; ++chan) {
		auto& env = m_EnvFilter.at(chan);
		auto& gain = m_GainFilter.at(chan);
		float temp = 0.0f, gainTemp = 1.0f;
		for (uint32_t fram = 0; fram < _renderFrames; ++fram) {
			//���͒l�̐�Βl�����[�p�X�Ɋ|���ĉ��������m
			temp = env.Process(std::fabsf(_buffer[chan][fram]));

			//���������Ƃɉ���(�Q�C��)�𒲐�
			gainTemp = 1.0f;

			if (temp > m_Param.threshold) {
				//�X���b�V�����h�𒴂���Ɖ���(�Q�C��)�𒲐�(���k)
				gainTemp = m_Param.threshold + (temp - m_Param.threshold) / m_Param.ratio;
			}
			//����(�Q�C��)���}���ɕω����Ȃ��悤���[�p�X��ʂ�
			gainTemp = gain.Process(gainTemp);

			//���͂ɉ���(�Q�C��)���|���A����ɍŏI�I�ȉ��ʂ𒲐߂��o��
			if (GetEnable())
				_buffer[chan][fram] = m_Param.gain * _buffer[chan][fram];
		}
	}
}

void AS::Compressor::Flush() {
	for (auto& env : m_EnvFilter)
		env.Flush();
	for (auto& gain : m_GainFilter)
		gain.Flush();
}