#include "FIRFilter.h"

AS::FIRFilter::FIRFilter() {
}

AS::FIRFilter::~FIRFilter() {
}

void AS::FIRFilter::SetFilterParam(FilterParamBase& _filterParam) {
	FIRParam& param = static_cast<FIRParam&>(_filterParam);
	m_WindowSelector = param.windowType;
	switch (param.filterType) {
	case FIRFilterType::AS_FIRTYPE_LPF:
		LowPass(param.lpfParam);
		break;
	case FIRFilterType::AS_FIRTYPE_HPF:
		HighPass(param.hpfParam);
		break;
	case FIRFilterType::AS_FIRTYPE_BPF:
		BandPass(param.bpfParam);
		break;
	case FIRFilterType::AS_FIRTYPE_BEF:
		BandEliminate(param.befParam);
		break;
	default:
		break;
	}
}

void AS::FIRFilter::Process(float* _src, int32_t _renderFrames) {
	//FIR
	if (m_FIRCoefficients.empty() || m_FIRFeedbackBuffer.capacity() == 0)return;

	for (int32_t fram = 0; fram < _renderFrames; ++fram) {
		auto dest = 0.0;
		for (size_t cntCoef = 0; cntCoef <= m_CntCoefTaps; ++cntCoef) {
			if (static_cast<int32_t>(m_FIRFeedbackBuffer.size() - cntCoef) > 0)
				dest += (m_FIRCoefficients.at(cntCoef) * m_WindowFuncCoefficients.at(cntCoef)) * m_FIRFeedbackBuffer.at(cntCoef);
		}
		m_FIRFeedbackBuffer.push_front(_src[fram]);
		_src[fram] = static_cast<float>(dest);
	}
}

void AS::FIRFilter::Flush() {
}

void AS::FIRFilter::ChangeWindowFunc(WindowFuncType _window) {
	m_WindowSelector = _window;
}

void AS::FIRFilter::LowPass(int32_t _samplingFreq, float _cutoffFreq, float _bandwidth) {
	//エッジ周波数
	float ef = _cutoffFreq / _samplingFreq;
	//遷移帯域幅
	float delta = _bandwidth / _samplingFreq;
	//遅延機数
	m_CntCoefTaps = calctaps(delta);
	//フィードバック用バッファを作成
	m_FIRFeedbackBuffer = boost::circular_buffer<double>(m_CntCoefTaps + 1);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_FIRCoefficients = std::vector<double>(m_CntCoefTaps + 1);
	m_WindowFuncCoefficients = std::vector<double>(m_CntCoefTaps + 1);

	for (size_t i = 0; i <= m_CntCoefTaps; ++i) {
		//b(m) = 2 * fe * sinc(2 * pi * fe * m) (-inf <= m <= inf)
		m_FIRCoefficients.at(i) = 2.0 * ef * sinc(2.0 * std::numbers::pi * ef * m(i));
	}

	WindowFunc(m_WindowFuncCoefficients, m_WindowSelector);
}

void AS::FIRFilter::HighPass(int32_t _samplingFreq, float _cutoffFreq, float _bandwidth) {
	//エッジ周波数
	float ef = _cutoffFreq / _samplingFreq;
	//遷移帯域幅
	float delta = _bandwidth / _samplingFreq;
	//遅延機数
	m_CntCoefTaps = calctaps(delta);
	//フィードバック用バッファを作成
	m_FIRFeedbackBuffer = boost::circular_buffer<double>(m_CntCoefTaps + 1);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_FIRCoefficients = std::vector<double>(m_CntCoefTaps + 1);
	m_WindowFuncCoefficients = std::vector<double>(m_CntCoefTaps + 1);

	for (size_t i = 0; i <= m_CntCoefTaps; ++i) {
		//b(m) = sinc(pi * m) - 2 * fe * sinc(2 * pi * m) (-inf <= m <= inf)
		m_FIRCoefficients.at(i) = sinc(std::numbers::pi * m(i)) - 2 * ef * sinc(2.0 * std::numbers::pi * ef * m(i));
	}

	WindowFunc(m_WindowFuncCoefficients, m_WindowSelector);
}

void AS::FIRFilter::BandPass(int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth) {
	//エッジ周波数
	float ef1 = _cutoffFreq1 / _samplingFreq;
	float ef2 = _cutoffFreq2 / _samplingFreq;
	//遷移帯域幅
	float delta = _bandwidth / _samplingFreq;
	//遅延機数
	m_CntCoefTaps = calctaps(delta);
	//フィードバック用バッファを作成
	m_FIRFeedbackBuffer = boost::circular_buffer<double>(m_CntCoefTaps + 1);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_FIRCoefficients = std::vector<double>(m_CntCoefTaps + 1);
	m_WindowFuncCoefficients = std::vector<double>(m_CntCoefTaps + 1);

	for (size_t i = 0; i <= m_CntCoefTaps; ++i) {
		//b(m) = 2 * fe2 *sinc(2 * pi * fe2 * m) - 2 * fe1 * sinc(2 * pi * fe1 * m) (-inf <= m <= inf)
		m_FIRCoefficients.at(i) = 2.0 * ef2 * sinc(2.0 * std::numbers::pi * ef2 * m(i))
			- 2.0 * ef1 * sinc(2.0 * std::numbers::pi * ef1 * m(i));
	}

	WindowFunc(m_WindowFuncCoefficients, m_WindowSelector);
}

void AS::FIRFilter::BandEliminate(int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth) {
	//エッジ周波数
	float ef1 = _cutoffFreq1 / _samplingFreq;
	float ef2 = _cutoffFreq2 / _samplingFreq;
	//遷移帯域幅
	float delta = _bandwidth / _samplingFreq;
	//遅延機数
	m_CntCoefTaps = calctaps(delta);
	//フィードバック用バッファを作成
	m_FIRFeedbackBuffer = boost::circular_buffer<double>(m_CntCoefTaps + 1);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_FIRCoefficients = std::vector<double>(m_CntCoefTaps + 1);
	m_WindowFuncCoefficients = std::vector<double>(m_CntCoefTaps + 1);

	for (size_t i = 0; i <= m_CntCoefTaps; ++i) {
		//b(m) = sinc(pi * m) - 2 * fe2 * sinc(2 * pi *fe2 * m) + 2 * fe1 * sinc(2 * pi *fe1 * m) (-inf <= m <= inf)
		m_FIRCoefficients.at(i) = sinc(std::numbers::pi * m(i))
			- 2.0 * ef2 * sinc(2.0 * std::numbers::pi * ef2 * m(i))
			+ 2.0 * ef1 * sinc(2.0 * std::numbers::pi * ef1 * m(i));
	}

	WindowFunc(m_WindowFuncCoefficients, m_WindowSelector);
}

double AS::FIRFilter::sinc(double _x) {
	return _x == 0.0 ? 1.0 : std::sin(_x) / _x;
}

int32_t AS::FIRFilter::calctaps(double _delta) {
	int32_t d = std::round(3.1 / _delta) - 1;
	//d+1が奇数となるよう調整
	return (d + 1) % 2 == 1 ? d++ : d;
}