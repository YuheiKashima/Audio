#ifndef _EFFECTMANAGER_
#define _EFFECTMANAGER_

#include "AudioDefines.h"
#include "EffectBase.h"
#include "ParallelEffector.h"

namespace AS {
	class EffectManager {
		template<typename T>
		friend class StdPtrHelper;
		friend class AudioSystem;
	public:
		template<class E>
		std::weak_ptr<E> AddEffect();
		template<class E, size_t SIZE>
		std::weak_ptr<TEMPLATE::ParallelEffector<E, SIZE>> AddParallel();

		void Execute(LineBuffer<float>& _buffer, const uint32_t _processFrames);
		const size_t GetEffectSize() { return m_Effects.size(); }
		void Flush();

	private:
		EffectManager(AudioFormat _format);
		~EffectManager();

		static std::shared_ptr<EffectManager> CreateInstance(AudioFormat _format) {
			return StdPtrHelper<EffectManager>::make_shared(_format);
		}

		std::vector<std::shared_ptr<EffectBase>> m_Effects;
		AudioFormat m_Format;
	};

	template<class E>
	std::weak_ptr<E> EffectManager::AddEffect() {
		static_assert(std::is_base_of<EffectBase, E>::value, "The specified type is not a derived type of CEffectBase");
		auto instance = std::make_shared<E>(m_Format);
		m_Effects.push_back(instance);
		return instance;
	}

	template<class E, size_t SIZE >
	std::weak_ptr<TEMPLATE::ParallelEffector<E, SIZE>> EffectManager::AddParallel() {
		static_assert(std::is_base_of<EffectBase, E>::value, "The specified type is not a derived type of CEffectBase");
		auto instance = std::make_shared<TEMPLATE::ParallelEffector<E, SIZE>>(m_Format);
		m_Effects.push_back(instance);
		return instance;
	}
}
#endif