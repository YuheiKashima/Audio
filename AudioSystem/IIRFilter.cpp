#include "Equalizer.h"

AS::IIRFilter::IIRFilter() {
}

AS::IIRFilter::~IIRFilter() {
}

void AS::IIRFilter::SetFilterParam(FilterParamBase& _filterParam) {
	IIRParam& param = static_cast<IIRParam&>(_filterParam);
	switch (param.filterType) {
	case IIRFilterType::AS_IIRTYPE_LPF:
		LowPass(param.lpfParam);
		break;
	case IIRFilterType::AS_IIRTYPE_HPF:
		HighPass(param.hpfParam);
		break;
	case IIRFilterType::AS_IIRTYPE_BPF:
		BandPass(param.bpfParam);
		break;
	case IIRFilterType::AS_IIRTYPE_BEF:
		BandEliminate(param.befParam);
		break;
	case IIRFilterType::AS_IIRTYPE_LSF:
		LowShelf(param.lsfParam);
		break;
	case IIRFilterType::AS_IIRTYPE_HSF:
		HighShelf(param.hsfParam);
		break;
	case IIRFilterType::AS_IIRTYPE_PEAK:
		Peaking(param.peakParam);
		break;
	case IIRFilterType::AS_IIRTYPE_APF:
		AllPass(param.apfParam);
		break;
	default:
		break;
	}
}

void AS::IIRFilter::Process(float* _src, int32_t _renderFrames) {
	for (int32_t fram = 0; fram < _renderFrames; ++fram) {
		double dest = m_BiCoef.beta[0] / m_BiCoef.alpha[0] * _src[fram] +
			m_BiCoef.beta[1] / m_BiCoef.alpha[0] * m_IIRBuff.in[0] +
			m_BiCoef.beta[2] / m_BiCoef.alpha[0] * m_IIRBuff.in[1] -
			m_BiCoef.alpha[1] / m_BiCoef.alpha[0] * m_IIRBuff.out[0] -
			m_BiCoef.alpha[2] / m_BiCoef.alpha[0] * m_IIRBuff.out[1];

		m_IIRBuff.in[1] = m_IIRBuff.in[0];
		m_IIRBuff.in[0] = _src[fram];

		m_IIRBuff.out[1] = m_IIRBuff.out[0];
		m_IIRBuff.out[0] = dest;

		_src[fram] = static_cast<float>(dest);
	}
}

void AS::IIRFilter::Flush() {
	m_IIRBuff.Flush();
}

void AS::IIRFilter::LowPass(int32_t _samplingFreq, double _cutoffFreq, double _q) {
	const double omega = 2.0f * static_cast<double>(M_PI) * _cutoffFreq / _samplingFreq;
	const double alpha = sin(omega) / (2.0f * _q);

	const double cosOmega = cos(omega);

	m_BiCoef = BiquadCoefficient(
		1.0f + alpha,
		-2.0f * cosOmega,
		1.0f - alpha,
		(1.0f - cosOmega) / 2.0f,
		1.0f - cosOmega,
		(1.0f - cosOmega) / 2.0f
	);
}
void AS::IIRFilter::HighPass(int32_t _samplingFreq, double _cutoffFreq, double _q) {
	const double omega = 2.0f * static_cast<double>(M_PI) * _cutoffFreq / _samplingFreq;
	const double alpha = sin(omega) / (2.0f * _q);

	const double cosOmega = cos(omega);

	m_BiCoef = BiquadCoefficient(
		1.0f + alpha,
		-2.0f * cosOmega,
		1.0f - alpha,
		(1.0f + cosOmega) / 2.0f,
		-(1.0f + cosOmega),
		(1.0f + cosOmega) / 2.0f
	);
}
void AS::IIRFilter::BandPass(int32_t _samplingFreq, double _centerFreq, double _bandwidth) {
	const double omega = 2.0f * static_cast<double>(M_PI) * _centerFreq / _samplingFreq;
	const double alpha = sin(omega) * sinh(log(2.0f) / 2.0f * _bandwidth * omega / sin(omega));

	const double cosOmega = cos(omega);

	m_BiCoef = BiquadCoefficient(
		1.0f + alpha,
		-2.0f * cosOmega,
		1.0f - alpha,
		alpha,
		0.0f,
		-alpha
	);
}
void AS::IIRFilter::BandEliminate(int32_t _samplingFreq, double _centerFreq, double _bandwidth) {
	const double omega = 2.0f * static_cast<double>(M_PI) * _centerFreq / _samplingFreq;
	const double alpha = sin(omega) * sinh(log(2.0f) / 2.0f * _bandwidth * omega / sin(omega));

	const double cosOmega = cos(omega);

	m_BiCoef = BiquadCoefficient(
		1.0f + alpha,
		-2.0f * cosOmega,
		1.0f - alpha,
		1.0f,
		-2.0f * cosOmega,
		1.0f
	);
}
void AS::IIRFilter::LowShelf(int32_t _samplingFreq, double _cutoffFreq, double _q, double _gain) {
	const double omega = 2.0f * static_cast<double>(M_PI) * _cutoffFreq / _samplingFreq;
	const double alpha = sin(omega) / (2.0f * _q);

	const double ampli = pow(10.0f, (_gain / 40.0f));
	const double beta = sqrt(ampli) / _q;

	const double cosOmega = cos(omega);
	const double sinOmega = sin(omega);

	m_BiCoef = BiquadCoefficient(
		(ampli + 1.0f) + (ampli - 1.0f) * cosOmega + beta * sinOmega,
		-2.0f * ((ampli - 1.0f) + (ampli + 1.0f) * cosOmega),
		(ampli + 1.0f) + (ampli - 1.0f) * cosOmega - beta * sinOmega,
		ampli * ((ampli + 1.0f) - (ampli - 1.0f) * cosOmega + beta * sinOmega),
		2.0f * ampli * ((ampli - 1.0f) - (ampli + 1.0f) * cosOmega),
		ampli * (ampli + 1.0f) - (ampli - 1.0f) * cosOmega - beta * sinOmega
	);
}
void AS::IIRFilter::HighShelf(int32_t _samplingFreq, double _cutoffFreq, double _q, double _gain) {
	const double omega = 2.0f * static_cast<double>(M_PI) * _cutoffFreq / _samplingFreq;
	const double alpha = sin(omega) / (2.0f * _q);

	const double ampli = pow(10.0f, (_gain / 40.0f));
	const double beta = sqrt(ampli) / _q;

	const double cosOmega = cos(omega);
	const double sinOmega = sin(omega);

	m_BiCoef = BiquadCoefficient(
		(ampli + 1.0f) - (ampli - 1.0f) * cosOmega + beta * sinOmega,
		2.0f * ((ampli - 1.0f) - (ampli + 1.0f) * cosOmega),
		(ampli + 1.0f) - (ampli - 1.0f) * cosOmega - beta * sinOmega,
		ampli * ((ampli + 1.0f) + (ampli - 1.0f) * cosOmega + beta * sinOmega),
		-2.0f * ampli * ((ampli - 1.0f) + (ampli + 1.0f) * cosOmega),
		ampli * (ampli + 1.0f) + (ampli - 1.0f) * cosOmega - beta * sinOmega
	);
}
void AS::IIRFilter::Peaking(int32_t _samplingFreq, double _centerFreq, double _bandwidth, double _gain) {
	const double omega = 2.0f * static_cast<double>(M_PI) * _centerFreq / _samplingFreq;
	const double alpha = sin(omega) * sinh(log(2.0f) / 2.0f * _bandwidth * omega / sin(omega));

	const double ampli = pow(10.0f, (_gain / 40.0f));

	const double cosOmega = cos(omega);
	const double sinOmega = sin(omega);

	m_BiCoef = BiquadCoefficient(
		1.0f + alpha / ampli,
		-2.0f * cosOmega,
		1.0f - alpha / ampli,
		1.0f + alpha * ampli,
		-2.0f * cosOmega,
		1.0f - alpha * ampli
	);
}
void AS::IIRFilter::AllPass(int32_t _samplingFreq, double _centerFreq, double _q) {
	const double omega = 2.0f * static_cast<double>(M_PI) * _centerFreq / _samplingFreq;
	const double alpha = sin(omega) / (2.0f * _q);

	const double cosOmega = cos(omega);
	const double sinOmega = sin(omega);

	m_BiCoef = BiquadCoefficient(
		1.0f + alpha,
		-2.0f * cosOmega,
		1.0f - alpha,
		1.0f - alpha,
		-2.0f * cosOmega,
		1.0f + alpha
	);
}