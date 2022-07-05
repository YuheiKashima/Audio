#ifndef _EFFECTBASE_
#define _EFFECTBASE_

#include "AudioDefines.h"

namespace AS {
	class EffectParamBase {};

	class EffectBase {
	public:
		virtual void SetEffectParam(EffectParamBase& _param) = 0;
		virtual void Process(LineBuffer<float>& _buffer, const uint32_t _processFrames) = 0;
		virtual void Flush() = 0;
		virtual void SetEnable(const bool _enable) { m_bEnable = m_bEnable != _enable ? _enable : m_bEnable; }
		const bool GetEnable() { return m_bEnable; }
	protected:
		EffectBase(AudioFormat _format) :m_Format(_format) { }
		virtual ~EffectBase() {}

		std::mutex m_ParamMutex;
		AudioFormat m_Format;
	private:
		bool m_bEnable = false;

		template <class E>
		static std::shared_ptr<E> CreateInstance(AudioFormat _format);
	};

	template<class E>
	std::shared_ptr<E> EffectBase::CreateInstance(AudioFormat _format) {
		static_assert(std::is_base_of<EffectBase, E>::value, "The specified type is not a derived type of CEffectBase");
		return StdPtrHelper<E>::make_shared(_format);
	}
}

#endif 