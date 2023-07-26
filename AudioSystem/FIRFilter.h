#include "EffectBase.h"

//éQçl
//https://github.com/T-Sumida/FIRFilter

namespace AS {
	enum class FIRWindow {
		FIR_WINDOW_SINGBELL,
		FIR_WINDOW_HANNING,
		FIR_WINDOW_HAMMING,
		FIR_WINDOW_BLACKMAN
	};

	class FIRFilter {
	public:
		FIRFilter();
		~FIRFilter();

		void ChangeWindowFunc(FIRWindow _window);
		void Process(float* _src, uint32_t _renderFrames);
		void Flush();

		void LowPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq, float _bandwidth);
		void HighPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq, float _bandwidth);
		void BandPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth);
		void BandEliminate(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth);
	private:
		double sinc(double _x);
		int32_t calctaps(double _delta);

		void WindowFunc(std::vector<double>& _coefs, FIRWindow _window);

		void SingbellWindow(std::vector<double>& _coefs);
		void HanningWindow(std::vector<double>& _coefs);
		void HammingWindow(std::vector<double>& _coefs);
		void BlackmanWindow(std::vector<double>& _coefs);

		int32_t m_CntCoefTaps = 0;
		std::vector<double> m_FIRCoefficients;
		std::vector<double> m_WindowFuncCoefficients;
		FIRWindow m_WindowSelector = FIRWindow::FIR_WINDOW_HANNING;
	};
}