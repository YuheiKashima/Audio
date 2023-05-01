#include "EffectBase.h"

namespace AS {
	class FIRFilter {
	public:
		FIRFilter();
		~FIRFilter();

		float Process(float _src);
		void Flush();

		void LowPass(int32_t _samplingFreq, float _cutoffFreq, float _bandwidth);
		void HighPass(int32_t _samplingFreq, float _cutoffFreq, float _bandwidth);
		void BandPass(int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth);
		void BandEliminate(int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth);

	private:
		float sincf(float _x);
		int32_t calctaps(float _delta);

		void WindowFunc(std::vector<float>& _coefs);

		void SingbellWindow(std::vector<float>& _coefs);
		void HanningWindow(std::vector<float>& _coefs));
		void HammingWindow();
		void BlackmanWindow();

		int32_t m_CntCoefTaps = 0;
		std::vector<float> m_Coeficients;
	};
}