#include "FIRFilter.h"

AS::FIRFilter::FIRFilter() {
}

AS::FIRFilter::~FIRFilter() {
}

void AS::FIRFilter::Process(float* _src, uint32_t _renderFrames) {
	//FIR
	if (m_Coeficients.empty())return;

	for (uint32_t fram = 0; fram < _renderFrames; ++fram) {
		for (uint32_t cntCoef = 0; auto & coef : m_Coeficients) {
			if (fram - cntCoef >= 0)_src[fram] += _src[fram] * coef;
		}
	}
}

void AS::FIRFilter::Flush() {
}

/**
* LPFの係数を作成する
* @param _samplingFreq サンプリング周波数
* @param _cutofFreq カットオフ周波数
* @param _bandwidth 帯域幅
*/
void AS::FIRFilter::LowPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq, float _bandwidth) {
	//エッジ周波数
	float ef = _cutoffFreq / _samplingFreq;
	//遷移帯域幅
	float delta = _bandwidth / _samplingFreq;
	//遅延機数
	m_CntCoefTaps = calctaps(delta);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_CntCoefTaps++;
	m_Coeficients = std::vector<float>(m_CntCoefTaps);

	for (auto i = 0; auto & coef : m_Coeficients) {
		//b(m) = 2 * fe * sinc(2 * pi * fe * m) (-inf <= m <= inf)
		coef = 2.0 * ef * sincf(2.0 * std::numbers::pi * ef * m(i));
		++i;
	}

	WindowFunc(m_Coeficients, _window);
}

/**
* HPFの係数を作成する
* @param _samplingFreq サンプリング周波数
* @param _cutofFreq カットオフ周波数
* @param _bandwidth 帯域幅
*/
void AS::FIRFilter::HighPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq, float _bandwidth) {
	//エッジ周波数
	float ef = _cutoffFreq / _samplingFreq;
	//遷移帯域幅
	float delta = _bandwidth / _samplingFreq;
	//遅延機数
	m_CntCoefTaps = calctaps(delta);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_CntCoefTaps++;
	m_Coeficients = std::vector<float>(m_CntCoefTaps);

	for (int32_t i = 0; auto & coef:m_Coeficients) {
		//b(m) = sinc(pi * m) - 2 * fe * sinc(2 * pi * m) (-inf <= m <= inf)
		coef = sincf(std::numbers::pi * m(i)) - 2 * ef * sincf(2.0 * std::numbers::pi * ef * m(i));
		++i;
	}

	WindowFunc(m_Coeficients, _window);
}

/**
* BPFの係数を作成する
* @param _samplingFreq サンプリング周波数
* @param _cutofFreq1 カットオフ周波数1
* @param _cutofFreq2 カットオフ周波数2
* @param _bandwidth 帯域幅
*/
void AS::FIRFilter::BandPass(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth) {
	//エッジ周波数
	float ef1 = _cutoffFreq1 / _samplingFreq;
	float ef2 = _cutoffFreq2 / _samplingFreq;
	//遷移帯域幅
	float delta = _bandwidth / _samplingFreq;
	//遅延機数
	m_CntCoefTaps = calctaps(delta);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_CntCoefTaps++;
	m_Coeficients = std::vector<float>(m_CntCoefTaps);

	for (int32_t i = 0; auto & coef:m_Coeficients) {
		//b(m) = 2 * fe2 *sinc(2 * pi * fe2 * m) - 2 * fe1 * sinc(2 * pi * fe1 * m) (-inf <= m <= inf)
		coef = 2.0 * ef2 * sincf(2.0 * std::numbers::pi * ef2 * m(i))
			- 2.0 * ef1 * sincf(2.0 * std::numbers::pi * ef1 * m(i));
		++i;
	}

	WindowFunc(m_Coeficients, _window);
}

void AS::FIRFilter::BandEliminate(FIRWindow _window, int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth) {
	//エッジ周波数
	float ef1 = _cutoffFreq1 / _samplingFreq;
	float ef2 = _cutoffFreq2 / _samplingFreq;
	//遷移帯域幅
	float delta = _bandwidth / _samplingFreq;
	//遅延機数
	m_CntCoefTaps = calctaps(delta);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_CntCoefTaps++;
	m_Coeficients = std::vector<float>(m_CntCoefTaps);

	for (int32_t i = 0; auto & coef:m_Coeficients) {
		//b(m) = sinc(pi * m) - 2 * fe2 * sinc(2 * pi *fe2 * m) + 2 * fe1 * sinc(2 * pi *fe1 * m) (-inf <= m <= inf)
		coef = sincf(std::numbers::pi * m(i))
			- 2.0 * ef2 * sincf(2.0 * std::numbers::pi * ef2 * m(i))
			+ 2.0 * ef1 * sincf(2.0 * std::numbers::pi * ef1 * m(i));
	}
	WindowFunc(m_Coeficients, _window);
}

float AS::FIRFilter::sincf(float _x) {
	float y = 0.0f;

	if (_x == 0.0f)
		y = 1.0f;
	else
		y = std::sinf(_x) / _x;

	return y;
}

int32_t AS::FIRFilter::calctaps(float _delta) {
	int32_t d = std::round(3.1 / _delta) - 1;
	d = (d + 1) % 2 == 0 ? d++ : d;
	return d;
}

void AS::FIRFilter::WindowFunc(std::vector<float>& _coefs, FIRWindow _window) {
	switch (_window)
	{
	case AS::FIRFilter::FIRWindow::FIR_WINDOW_SINGBELL:
		SingbellWindow(_coefs);
		break;
	case AS::FIRFilter::FIRWindow::FIR_WINDOW_HANNING:
		HanningWindow(_coefs);
		break;
	case AS::FIRFilter::FIRWindow::FIR_WINDOW_HAMMING:
		HammingWindow(_coefs);
		break;
	case AS::FIRFilter::FIRWindow::FIR_WINDOW_BLACKMAN:
		BlackmanWindow(_coefs);
		break;
	default:
		break;
	}
}

void AS::FIRFilter::SingbellWindow(std::vector<float>& _coefs) {
	float f = 2.0f * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; auto & coef:_coefs) {
		coef *= std::sinf(f * i);
		++i;
	}
}

void AS::FIRFilter::HanningWindow(std::vector<float>& _coefs) {
	float f = 2.0f * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; auto & coef:_coefs) {
		coef *= 0.5f * (1.0f - std::cosf(f * i));
		++i;
	}
}

void AS::FIRFilter::HammingWindow(std::vector<float>& _coefs) {
	float f = 2.0f * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; auto & coef:_coefs) {
		coef *= 0.54 - 0.46 * std::cosf(f * i);
		++i;
	}
}

void AS::FIRFilter::BlackmanWindow(std::vector<float>& _coefs) {
	float f = 2.0f * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; auto & coef:_coefs) {
		coef *= 0.42f - 0.5f * std::cosf(f * i) + 0.08f * std::cosf(2.0f * f * i);
		++i;
	}
}