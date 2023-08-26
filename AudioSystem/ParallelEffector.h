#ifndef _EFFECTSPLITTER_
#define _EFFECTSPLITTER_
#include "EffectBase.h"

namespace AS {
	namespace VECTOR {
		class ParallelEffector :public EffectBase {
		public:
			ParallelEffector(AudioFormat _format) :EffectBase(_format) {};
			~ParallelEffector() {};

			void Process(LineBuffer<float>& _buffer, int32_t _renderFrames) override {
				m_DestTempBuffer = m_ProcessBuffer = _buffer;
				m_DestTempBuffer.zeroclear();
				for (auto parallel : m_ParallelEffects) {
					m_ProcessBuffer = _buffer;
					parallel->Process(m_ProcessBuffer, _renderFrames);
					m_DestTempBuffer.add(m_ProcessBuffer);
				}
				_buffer = m_DestTempBuffer;
			}

			void Flush() override {
				for (auto parallel : m_ParallelEffects) {
					parallel->Flush();
				}
			}

			template<class E>
			std::weak_ptr<E> AddEffect();

			size_t GetParallelSize() { return m_ParallelEffects.size(); }
		private:
			std::vector<std::shared_ptr<EffectBase>> m_ParallelEffects;
			LineBuffer<float> m_ProcessBuffer, m_DestTempBuffer;

			void SetEffectParam(EffectParamBase& _param) override {}
		};

		template<class E>
		std::weak_ptr<E> ParallelEffector::AddEffect() {
			static_assert(std::is_base_of<EffectBase, E>::value, "The specified type is not a derived type of CEffectBase");
			auto instance = std::make_shared<E>(m_Format);
			m_ParallelEffects.push_back(instance);
			return instance;
		}
	}
	namespace TEMPLATE {
		template<class EFFECT = EffectBase, size_t SIZE = 1>
		class ParallelEffector :public EffectBase {
		public:
			ParallelEffector(AudioFormat _format) :EffectBase(_format) {
				static_assert(std::is_base_of<EffectBase, EFFECT>::value, "The specified type is not a derived type of EffectBase");
				for (std::shared_ptr<EFFECT>& parallel : m_ParallelEffects) {
					parallel = std::make_shared<EFFECT>(m_Format);
				}
			};
			~ParallelEffector() {};

			void Process(LineBuffer<float>& _buffer, int32_t _renderFrames) override {
				m_DestTempBuffer = m_ProcessBuffer = _buffer;
				m_DestTempBuffer.zeroclear();
				for (std::shared_ptr<EFFECT> parallel : m_ParallelEffects) {
					m_ProcessBuffer = _buffer;
					parallel->Process(m_ProcessBuffer, _renderFrames);
					m_DestTempBuffer.add(m_ProcessBuffer);
				}
				if (GetEnable())
					_buffer = m_DestTempBuffer;
			}

			void Flush() override {
				for (std::shared_ptr<EFFECT> parallel : m_ParallelEffects) {
					parallel->Flush();
				}
			}

			size_t GetParallelSize() { return m_ParallelEffects.size(); }
			std::weak_ptr<EFFECT> At(size_t _i) { return m_ParallelEffects.at(_i); }
			std::array<std::shared_ptr<EFFECT>, SIZE> m_ParallelEffects;
			LineBuffer<float> m_ProcessBuffer, m_DestTempBuffer;

			void SetEffectParam(EffectParamBase& _param) override {}
		};
	}
}
#endif