#include "AllpassFilter.h"

//参考書
//Wizard Notes https://www.wizard-notes.com/
//株式会社ARI http://www.ari-web.com/service/soft/reverb-2.htm

AS::AllpassFilter::AllpassFilter(AudioFormat _format) :EffectBase(_format) {
}

AS::AllpassFilter::~AllpassFilter() {
}

void AS::AllpassFilter::SetEffectParam(EffectParamBase& _param) {
	std::lock_guard lock(m_ParamMutex);
	m_Param = static_cast<AllpassParam&>(_param);
	m_DelayLength = static_cast<int32_t>(m_Format.samplingRate * m_Param.delayTime / 1000.0);

	std::vector<boost::circular_buffer<double>> circularBuf(m_Format.channnels);
	for (auto& cir : circularBuf)
		cir.set_capacity(m_DelayLength);
	m_DelayBuffer = std::move(circularBuf);
}

void AS::AllpassFilter::Process(LineBuffer<float>& _buffer, int32_t _renderFrames) {
	std::lock_guard lock(m_ParamMutex);

	for (int32_t chan = 0; auto & cir:m_DelayBuffer) {
		double src = 0.0f, dest = 0.0f;
		for (int32_t fram = 0; fram < _renderFrames; ++fram) {
			src = _buffer[chan][fram];
			if (cir.full()) {
				double delaiedSignal = cir.front();
				double feedback = src + (delaiedSignal * m_Param.decayLevel);

				double feedforward = delaiedSignal + (feedback * (-m_Param.decayLevel));

				cir.push_back(feedback);
			}
			else {
				//遅延バッファが溜まっていなければ実質元データママだからケツに入れ込むだけ
				cir.push_back(src);
			}
		}
	}
}

void AS::AllpassFilter::Flush() {
	for (auto& cir : m_DelayBuffer)
		cir.clear();
}