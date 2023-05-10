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
	LineBuffer<float> copy = _buffer;

	for (uint32_t chan = 0; chan < m_Format.channnels; ++chan) {
		auto& env = m_EnvFilter.at(chan);
		auto& gain = m_GainFilter.at(chan);

		//入力値の絶対値をローパスに掛けて音圧を検知
		env.Process(&copy[chan].front(), _renderFrames);

		for (uint32_t fram = 0; fram < _renderFrames; ++fram) {
			if (copy[chan][fram] > m_Param.threshold) {
				//閾値を超えると音量(ゲイン)を調整(圧縮)
				copy[chan][fram] = m_Param.threshold + (copy[chan][fram] - m_Param.threshold) / m_Param.ratio;
			}
		}

		//音量(ゲイン)が急激に変化しないようローパスを通す
		gain.Process(&copy[chan].front(), _renderFrames);
	}

	copy.mul(m_Param.gain);

	//入力に音量(ゲイン)を掛け、さらに最終的な音量を調節し出力
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