#include "EffectBase.h"

namespace AS {
	class FIRFilter {
	public:
		FIRFilter();
		~FIRFilter();

		enum class FIRWindow {
			FIR_WINDOW_SINGBELL,
			FIR_WINDOW_HANNING,
			FIR_WINDOW_HAMMING,
			FIR_WINDOW_BLACKMAN
		};

		void Process(float* _src, uint32_t _renderFrames);
		void Flush();

		void LowPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq, float _bandwidth);
		void HighPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq, float _bandwidth);
		void BandPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth);
		void BandEliminate(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth);
	private:
		float sincf(float _x);
		int32_t calctaps(float _delta);

		void WindowFunc(std::vector<float>& _coefs, FIRWindow _window);

		void SingbellWindow(std::vector<float  >& _coefs);
		void HanningWindow(std::vector<float>& _coefs);
		void HammingWindow(std::vector<float>& _coefs);
		void BlackmanWindow(std::vector<float>& _coefs);

		int32_t m_CntCoefTaps = 0;
		std::vector<float> m_Coeficients;
		FIRWindow m_WindowSelector = FIRWindow::FIR_WINDOW_HANNING;
	};
}