#include "CombFilter.h"

AS::CombFilter::CombFilter(AudioFormat _format) :EffectBase(_format) {
}

AS::CombFilter::~CombFilter() {
}

void AS::CombFilter::SetEffectParam(EffectParamBase& _param) {
	std::lock_guard lock(m_ParamMutex);
	m_Param = static_cast<CombParam&>(_param);
	m_Delaylength = static_cast<int32_t>(m_Format.samplingRate * m_Param.delayTime / 1000.0);

	std::vector<boost::circular_buffer<double>> circularBuf(m_Format.channnels);
	for (auto& cir : circularBuf) {
		cir.set_capacity(m_Delaylength);
	}
	m_DelayBuffer = std::move(circularBuf);
}

void AS::CombFilter::Process(LineBuffer<float>& _buffer, int32_t _renderFrames) {
	std::lock_guard lock(m_ParamMutex);

	for (int32_t chan = 0; auto & cir:m_DelayBuffer) {
		float src = 0.0f, dest = 0.0f;
		for (int32_t fram = 0; fram < _renderFrames; ++fram) {
			src = _buffer[chan][fram];
			if (cir.full()) {
				//入力信号にディレイを信号を加える
				dest = src + (m_Param.decayLevel * cir.front());
				//入力＋ディレイを再度フィードバック用バッファに書き込み
				cir.push_back(dest);
				//出力
				_buffer[chan][fram] = GetEnable() ? dest : src;
			}
			else {
				//遅延バッファが溜まっていなければ実質元データママだからケツに入れ込むだけ
				cir.push_back(src);
			}
		}
		++chan;
	}
}

void AS::CombFilter::Flush() {
	for (auto& cir : m_DelayBuffer)
		cir.clear();
}