#ifndef _FILTEREFFECT_
#define _FILTEREFFECT_

#include "EffectBase.h"
#include "FIRFilter.h"
#include "IIRFilter.h"

namespace AS {
	struct FilterParam :public EffectParamBase {
		union {
			FIRParam firParam;
			IIRParam iirParam;
		};
		FilterParam(FIRParam& _param) :firParam(_param) {};
		FilterParam(IIRParam& _param) :iirParam(_param) {};
	};

	template<class Filter>
	class FilterEffect :public EffectBase {
	public:
		FilterEffect(AudioFormat _format) :EffectBase(_format) {}
		~FilterEffect() {}

		void SetEffectParam(EffectParamBase& _param)override {
			static_assert(typeid(Filter) == typeid(FIRFilter) || typeid(Filter) == typeid(IIRFilter)
				, "The specified type is not a supported filter. Please specify FIR or IIR filter");
			std::lock_guard lock(m_ParamMutex);
			FilterParam param = static_cast<FilterParam&>(_param);

			if (typeid(Filter) == typeid(FIRFilter)) {
				m_FilterEffect.SetFilterParam(param.firParam);
			}
			else if (typeid(Filter) == typeid(IIRFilter)) {
				m_FilterEffect.SetFilterParam(param.iirParam);
			}
		}

		void Process(LineBuffer<float>& _buffer, int32_t _renderFrames)override {
			std::lock_guard lock(m_ParamMutex);
			m_DestTempBuf = _buffer;

			for (auto chan = 0; chan < m_Format.channnels; ++chan) {
				m_FilterEffect.Process(&m_DestTempBuf.at(chan).front(), _renderFrames);
			}

			if (GetEnable()) {
				_buffer = m_DestTempBuf;
			}
		}

		void Flush()override {
			m_FilterEffect.Flush();
			m_DestTempBuf.Reset();
		}

	private:
		Filter m_FilterEffect;
		LineBuffer<float> m_DestTempBuf;
	};
}
#endif