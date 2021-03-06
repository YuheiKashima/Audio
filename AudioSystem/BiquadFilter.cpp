#include "BiquadFilter.h"

AS::BiquadFilter::BiquadFilter() {
}

AS::BiquadFilter::~BiquadFilter() {
}

float AS::BiquadFilter::Process(float _src) {
	float dest = m_BiCoef.beta[0] / m_BiCoef.alpha[0] * _src +
		m_BiCoef.beta[1] / m_BiCoef.alpha[0] * m_IIRBuff.in[0] +
		m_BiCoef.beta[2] / m_BiCoef.alpha[0] * m_IIRBuff.in[1] -
		m_BiCoef.alpha[1] / m_BiCoef.alpha[0] * m_IIRBuff.out[0] -
		m_BiCoef.alpha[2] / m_BiCoef.alpha[0] * m_IIRBuff.out[1];

	m_IIRBuff.in[1] = m_IIRBuff.in[0];
	m_IIRBuff.in[0] = _src;

	m_IIRBuff.out[1] = m_IIRBuff.out[0];
	m_IIRBuff.out[0] = dest;

	return dest;
}

void AS::BiquadFilter::Flush() {
	m_IIRBuff.Flush();
}

void AS::BiquadFilter::LowPass(uint32_t _samplingFreq, float _cutoffFreq, float _q) {
	const float omega = 2.0f * static_cast<float>(M_PI) * _cutoffFreq / _samplingFreq;
	const float alpha = sin(omega) / (2.0f * _q);

	const float cosOmega = cos(omega);

	m_BiCoef = BiquadCoeficients(
		1.0f + alpha,
		-2.0f * cosOmega,
		1.0f - alpha,
		(1.0f - cosOmega) / 2.0f,
		1.0f - cosOmega,
		(1.0f - cosOmega) / 2.0f
	);
}
void AS::BiquadFilter::HighPass(uint32_t _samplingFreq, float _cutoffFreq, float _q) {
	const float omega = 2.0f * static_cast<float>(M_PI) * _cutoffFreq / _samplingFreq;
	const float alpha = sin(omega) / (2.0f * _q);

	const float cosOmega = cos(omega);

	m_BiCoef = BiquadCoeficients(
		1.0f + alpha,
		-2.0f * cosOmega,
		1.0f - alpha,
		(1.0f + cosOmega) / 2.0f,
		-(1.0f + cosOmega),
		(1.0f + cosOmega) / 2.0f
	);
}
void AS::BiquadFilter::BandPass(uint32_t _samplingFreq, float _centerFreq, float _bandwidth) {
	const float omega = 2.0f * static_cast<float>(M_PI) * _centerFreq / _samplingFreq;
	const float alpha = sin(omega) * sinh(log(2.0f) / 2.0f * _bandwidth * omega / sin(omega));

	const float cosOmega = cos(omega);

	m_BiCoef = BiquadCoeficients(
		1.0f + alpha,
		-2.0f * cosOmega,
		1.0f - alpha,
		alpha,
		0.0f,
		-alpha
	);
}
void AS::BiquadFilter::Notch(uint32_t _samplingFreq, float _centerFreq, float _bandwidth) {
	const float omega = 2.0f * static_cast<float>(M_PI) * _centerFreq / _samplingFreq;
	const float alpha = sin(omega) * sinh(log(2.0f) / 2.0f * _bandwidth * omega / sin(omega));

	const float cosOmega = cos(omega);

	m_BiCoef = BiquadCoeficients(
		1.0f + alpha,
		-2.0f * cosOmega,
		1.0f - alpha,
		1.0f,
		-2.0f * cosOmega,
		1.0f
	);
}
void AS::BiquadFilter::LowShelf(uint32_t _samplingFreq, float _cutoffFreq, float _q, float _gain) {
	const float omega = 2.0f * static_cast<float>(M_PI) * _cutoffFreq / _samplingFreq;
	const float alpha = sin(omega) / (2.0f * _q);

	const float ampli = pow(10.0f, (_gain / 40.0f));
	const float beta = sqrt(ampli) / _q;

	const float cosOmega = cos(omega);
	const float sinOmega = sin(omega);

	m_BiCoef = BiquadCoeficients(
		(ampli + 1.0f) + (ampli - 1.0f) * cosOmega + beta * sinOmega,
		-2.0f * ((ampli - 1.0f) + (ampli + 1.0f) * cosOmega),
		(ampli + 1.0f) + (ampli - 1.0f) * cosOmega - beta * sinOmega,
		ampli * ((ampli + 1.0f) - (ampli - 1.0f) * cosOmega + beta * sinOmega),
		2.0f * ampli * ((ampli - 1.0f) - (ampli + 1.0f) * cosOmega),
		ampli * (ampli + 1.0f) - (ampli - 1.0f) * cosOmega - beta * sinOmega
	);
}
void AS::BiquadFilter::HighShelf(uint32_t _samplingFreq, float _cutoffFreq, float _q, float _gain) {
	const float omega = 2.0f * static_cast<float>(M_PI) * _cutoffFreq / _samplingFreq;
	const float alpha = sin(omega) / (2.0f * _q);

	const float ampli = pow(10.0f, (_gain / 40.0f));
	const float beta = sqrt(ampli) / _q;

	const float cosOmega = cos(omega);
	const float sinOmega = sin(omega);

	m_BiCoef = BiquadCoeficients(
		(ampli + 1.0f) - (ampli - 1.0f) * cosOmega + beta * sinOmega,
		2.0f * ((ampli - 1.0f) - (ampli + 1.0f) * cosOmega),
		(ampli + 1.0f) - (ampli - 1.0f) * cosOmega - beta * sinOmega,
		ampli * ((ampli + 1.0f) + (ampli - 1.0f) * cosOmega + beta * sinOmega),
		-2.0f * ampli * ((ampli - 1.0f) + (ampli + 1.0f) * cosOmega),
		ampli * (ampli + 1.0f) + (ampli - 1.0f) * cosOmega - beta * sinOmega
	);
}
void AS::BiquadFilter::Peaking(uint32_t _samplingFreq, float _centerFreq, float _bandwidth, float _gain) {
	const float omega = 2.0f * static_cast<float>(M_PI) * _centerFreq / _samplingFreq;
	const float alpha = sin(omega) * sinh(log(2.0f) / 2.0f * _bandwidth * omega / sin(omega));

	const float ampli = pow(10.0f, (_gain / 40.0f));

	const float cosOmega = cos(omega);
	const float sinOmega = sin(omega);

	m_BiCoef = BiquadCoeficients(
		1.0f + alpha / ampli,
		-2.0f * cosOmega,
		1.0f - alpha / ampli,
		1.0f + alpha * ampli,
		-2.0f * cosOmega,
		1.0f - alpha * ampli
	);
}
void AS::BiquadFilter::AllPass(uint32_t _samplingFreq, float _centerFreq, float _q) {
	const float omega = 2.0f * static_cast<float>(M_PI) * _centerFreq / _samplingFreq;
	const float alpha = sin(omega) / (2.0f * _q);

	const float cosOmega = cos(omega);
	const float sinOmega = sin(omega);

	m_BiCoef = BiquadCoeficients(
		1.0f + alpha,
		-2.0f * cosOmega,
		1.0f - alpha,
		1.0f - alpha,
		-2.0f * cosOmega,
		1.0f + alpha
	);
}