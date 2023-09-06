#include "FilterBase.h"

void AS::FilterBase::CreateWindowFunction(WindowFuncType _type, std::vector<double>& _destWindowCoeff) {
	switch (_type) {
	case AS::WindowFuncType::AS_WINDOWFUNC_RECTANGULAR:
		RecctangularWindow(_destWindowCoeff);
		break;
	case AS::WindowFuncType::AS_WINDOWFUNC_SINGBELL:
		SingbellWindow(_destWindowCoeff);
		break;
	case AS::WindowFuncType::AS_WINDOWFUNC_HANNING:
		HanningWindow(_destWindowCoeff);
		break;
	case AS::WindowFuncType::AS_WINDOWFUNC_HAMMING:
		HammingWindow(_destWindowCoeff);
		break;
	case AS::WindowFuncType::AS_WINDOWFUNC_BLACKMAN:
		BlackmanWindow(_destWindowCoeff);
		break;
	default:
		break;
	}
}

void AS::FilterBase::CreateWindowFuncMultiplyCoeffitient(std::vector<double>& _coef, WindowFuncType _type) {
	std::vector<double> window(_coef.size());
	CreateWindowFunction(_type, window);
	for (int32_t i = 0; auto & win:window) {
		_coef.at(i++) *= win;
	}
}

void AS::FilterBase::RecctangularWindow(std::vector<double>& _coefs) {
	for (size_t i = 0; i < _coefs.size(); ++i) {
		_coefs.at(i) = 1.0;
	}
}

void AS::FilterBase::SingbellWindow(std::vector<double>& _coefs) {
	const double f = 2.0 * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; i < _coefs.size(); ++i) {
		_coefs.at(i) = std::sin(f * i);
	}
}

void AS::FilterBase::HanningWindow(std::vector<double>& _coefs) {
	const double f = 2.0 * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; i < _coefs.size(); ++i) {
		_coefs.at(i) = 0.5 - 0.5 * std::cos(f * i);
	}
}

void AS::FilterBase::HammingWindow(std::vector<double>& _coefs) {
	const double f = 2.0 * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; i < _coefs.size(); ++i) {
		_coefs.at(i) = 0.54 - 0.46 * std::cos(f * i);
	}
}

void AS::FilterBase::BlackmanWindow(std::vector<double>& _coefs) {
	const double f = 2.0 * std::numbers::pi / static_cast<float>(_coefs.size() - 1);

	for (size_t i = 0; i < _coefs.size(); ++i) {
		_coefs.at(i) = 0.42 - 0.5 * std::cos(f * i) + 0.08 * std::cosf(2.0 * f * i);
	}
}