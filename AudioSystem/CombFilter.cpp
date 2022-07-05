#include "CombFilter.h"

AS::CombFilter::CombFilter(AudioFormat _format) :EffectBase(_format) {
}

AS::CombFilter::~CombFilter() {
}

void AS::CombFilter::SetEffectParam(EffectParamBase& _param) {
	std::lock_guard lock(m_ParamMutex);
	m_Param = static_cast<CombParam&>(_param);

	//Create CircularBuffer
	std::vector<CircularBuffer<float>> circularBuf(m_Format.channnels);
	m_DelayPoint = static_cast<uint32_t>(m_Format.samplingRate * m_Param.delayTime / 1000.0f);
	for (auto& cir : circularBuf) {
		cir.resize(static_cast<size_t>(m_DelayPoint));
	}
	m_CircularBuffers = std::move(circularBuf);
}

void AS::CombFilter::Process(LineBuffer<float>& _buffer, uint32_t _renderFrames) {
	std::lock_guard lock(m_ParamMutex);
	for (uint32_t chan = 0; auto & cir:m_CircularBuffers) {
		float src = 0.0f, dest = 0.0f;
		for (uint32_t fram = 0; fram < _renderFrames; ++fram) {
			src = _buffer[chan][fram];
			//���͐M���Ƀf�B���C�M����������
			dest = src + (m_Param.decayLevel * cir[0]);
			//�f�B���C�M���Ƃ��ē��͐M��(src)�ƍ���̃f�B���C�M�����t�B�[�h�o�b�N�������s���ăo�b�t�@�ɏ�������
			cir[m_DelayPoint] = src + (m_Param.feedBack * cir[0]);
			//�o��
			_buffer[chan][fram] = GetEnable() ? dest : src;

			//�z�o�b�t�@�X�V
			++cir;
		}
		++chan;
	}
}

void AS::CombFilter::Flush() {
	for (auto& cir : m_CircularBuffers)
		cir.clear();
}