#include "FIRFilter.h"

AS::FIRFilter::FIRFilter() :m_WindowSelector(FIRWindow::FIR_WINDOW_HANNING) {
}

AS::FIRFilter::~FIRFilter() {
}

void AS::FIRFilter::ChangeWindowFunc(FIRWindow _window) {
	m_WindowSelector = _window;
}

void AS::FIRFilter::Process(float* _src, uint32_t _renderFrames) {
	//FIR
	if (m_FIRCoefficients.empty())return;

	for (uint32_t fram = 0; fram < _renderFrames; ++fram) {
		for (uint32_t cntCoef = 0; auto & coef : m_FIRCoefficients) {
			if (fram - cntCoef >= 0)
				_src[fram] += coef * _src[fram - cntCoef];
		}
	}
}

void AS::FIRFilter::Flush() {
}

void AS::FIRFilter::LowPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq, float _bandwidth) {
	//ÉGÉbÉWé¸îgêî
	float ef = _cutoffFreq / _samplingFreq;
	//ëJà⁄ë—àÊïù
	float delta = _bandwidth / _samplingFreq;
	//íxâÑã@êî
	m_CntCoefTaps = calctaps(delta);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_CntCoefTaps++;
	m_FIRCoefficients = std::vector<double>(m_CntCoefTaps);

	for (auto i = 0; auto & coef : m_FIRCoefficients) {
		//b(m) = 2 * fe * sinc(2 * pi * fe * m) (-inf <= m <= inf)
		coef = 2.0 * ef * sinc(2.0 * std::numbers::pi * ef * m(i));
		++i;
	}

	WindowFunc(m_FIRCoefficients, _window);
}

void AS::FIRFilter::HighPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq, float _bandwidth) {
	//ÉGÉbÉWé¸îgêî
	float ef = _cutoffFreq / _samplingFreq;
	//ëJà⁄ë—àÊïù
	float delta = _bandwidth / _samplingFreq;
	//íxâÑã@êî
	m_CntCoefTaps = calctaps(delta);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_CntCoefTaps++;
	m_FIRCoefficients = std::vector<double>(m_CntCoefTaps);

	for (int32_t i = 0; auto & coef:m_FIRCoefficients) {
		//b(m) = sinc(pi * m) - 2 * fe * sinc(2 * pi * m) (-inf <= m <= inf)
		coef = sinc(std::numbers::pi * m(i)) - 2 * ef * sinc(2.0 * std::numbers::pi * ef * m(i));
		++i;
	}

	WindowFunc(m_FIRCoefficients, _window);
}

void AS::FIRFilter::BandPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth) {
	//ÉGÉbÉWé¸îgêî
	float ef1 = _cutoffFreq1 / _samplingFreq;
	float ef2 = _cutoffFreq2 / _samplingFreq;
	//ëJà⁄ë—àÊïù
	float delta = _bandwidth / _samplingFreq;
	//íxâÑã@êî
	m_CntCoefTaps = calctaps(delta);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_CntCoefTaps++;
	m_FIRCoefficients = std::vector<double>(m_CntCoefTaps);

	for (int32_t i = 0; auto & coef:m_FIRCoefficients) {
		//b(m) = 2 * fe2 *sinc(2 * pi * fe2 * m) - 2 * fe1 * sinc(2 * pi * fe1 * m) (-inf <= m <= inf)
		coef = 2.0 * ef2 * sinc(2.0 * std::numbers::pi * ef2 * m(i))
			- 2.0 * ef1 * sinc(2.0 * std::numbers::pi * ef1 * m(i));
		++i;
	}

	WindowFunc(m_FIRCoefficients, _window);
}

void AS::FIRFilter::BandEliminate(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth) {
	//ÉGÉbÉWé¸îgêî
	float ef1 = _cutoffFreq1 / _samplingFreq;
	float ef2 = _cutoffFreq2 / _samplingFreq;
	//ëJà⁄ë—àÊïù
	float delta = _bandwidth / _samplingFreq;
	//íxâÑã@êî
	m_CntCoefTaps = calctaps(delta);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_CntCoefTaps++;
	m_FIRCoefficients = std::vector<double>(m_CntCoefTaps);

	for (int32_t i = 0; auto & coef:m_FIRCoefficients) {
		//b(m) = sinc(pi * m) - 2 * fe2 * sinc(2 * pi *fe2 * m) + 2 * fe1 * sinc(2 * pi *fe1 * m) (-inf <= m <= inf)
		coef = sinc(std::numbers::pi * m(i))
			- 2.0 * ef2 * sinc(2.0 * std::numbers::pi * ef2 * m(i))
			+ 2.0 * ef1 * sinc(2.0 * std::numbers::pi * ef1 * m(i));
	}
	WindowFunc(m_FIRCoefficients, _window);
}

double AS::FIRFilter::sinc(double _x) {
	return _x == 0.0 ? 1.0 : std::sin(_x) / _x;
}

int32_t AS::FIRFilter::calctaps(double _delta) {
	int32_t d = std::round(3.1 / _delta) - 1;
	return (d + 1) % 2 == 0 ? d++ : d;
}

void AS::FIRFilter::WindowFunc(std::vector<double>& _coefs, FIRWindow _window) {
	switch (_window)
	{
	case AS::FIRWindow::FIR_WINDOW_SINGBELL:
		SingbellWindow(_coefs);
		break;
	case AS::FIRWindow::FIR_WINDOW_HANNING:
		HanningWindow(_coefs);
		break;
	case AS::FIRWindow::FIR_WINDOW_HAMMING:
		HammingWindow(_coefs);
		break;
	case AS::FIRWindow::FIR_WINDOW_BLACKMAN:
		BlackmanWindow(_coefs);
		break;
	default:
		break;
	}
}

void AS::FIRFilter::SingbellWindow(std::vector<double>& _coefs) {
	float f = 2.0f * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; auto & coef:_coefs) {
		coef *= std::sinf(f * i);
		++i;
	}
}

void AS::FIRFilter::HanningWindow(std::vector<double>& _coefs) {
	float f = 2.0f * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; auto & coef:_coefs) {
		coef *= 0.5f * (1.0f - std::cosf(f * i));
		++i;
	}
}

void AS::FIRFilter::HammingWindow(std::vector<double>& _coefs) {
	float f = 2.0f * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; auto & coef:_coefs) {
		coef *= 0.54 - 0.46 * std::cosf(f * i);
		++i;
	}
}

void AS::FIRFilter::BlackmanWindow(std::vector<double>& _coefs) {
	float f = 2.0f * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; auto & coef:_coefs) {
		coef *= 0.42f - 0.5f * std::cosf(f * i) + 0.08f * std::cosf(2.0f * f * i);
		++i;
	}
}