#include "EffectManager.h"

AS::EffectManager::EffectManager(AudioFormat _format) :m_Format(_format) {
}

AS::EffectManager::~EffectManager() {
}

void AS::EffectManager::Execute(LineBuffer<float>& _buffer, const int32_t _processFrames) {
	if (!m_Effects.size())return;
	for (auto& effect : m_Effects) {
		effect->Process(_buffer, _processFrames);
	}
}

void AS::EffectManager::Flush() {
	if (!m_Effects.size())return;
	for (auto& effect : m_Effects) {
		effect->Flush();
	}
}