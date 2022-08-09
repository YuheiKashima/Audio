#include "CombFilter.h"

AS::CombFilter::CombFilter(AudioFormat _format) :EffectBase(_format) {
}

AS::CombFilter::~CombFilter() {
}

void AS::CombFilter::SetEffectParam(EffectParamBase& _param) {
	std::lock_guard lock(m_ParamMutex);
	m_Param = static_cast<CombParam&>(_param);
	m_DelayPoint = static_cast<uint32_t>(m_Format.samplingRate * m_Param.delayTime / 1000.0f);

	std::vector<boost::circular_buffer<float>> circularBuf(m_Format.channnels);
	for (auto& cir : circularBuf) {
		cir.set_capacity(m_DelayPoint);
	}
	m_CircularBuffers = std::move(circularBuf);
}

void AS::CombFilter::Process(LineBuffer<float>& _buffer, uint32_t _renderFrames) {
	std::lock_guard lock(m_ParamMutex);

	for (uint32_t chan = 0; auto & cir:m_CircularBuffers) {
		float src = 0.0f, dest = 0.0f;
		for (uint32_t fram = 0; fram < _renderFrames; ++fram) {
			src = _buffer[chan][fram];
			if (cir.full()) {
				float front = 0.0f;

				//���͐M���Ƀf�B���C��M����������
				dest = src + (m_Param.decayLevel * cir.front());
				//�f�B���C�M���Ƃ��ē��͐M��(src)�ƍ���̃f�B���C�M�����t�B�[�h�o�b�N�������s���ăo�b�t�@�ɏ�������
				cir.push_back(src + m_Param.feedBack * cir.front());
				//�o��
				_buffer[chan][fram] = GetEnable() ? dest : src;
			}
			else {
				//�x���o�b�t�@�����܂��Ă��Ȃ���Ύ������f�[�^�}�}������P�c�ɓ��ꍞ�ނ���
				cir.push_back(src);
			}
		}
		++chan;
	}
}

void AS::CombFilter::Flush() {
	for (auto& cir : m_CircularBuffers)
		cir.clear();
}