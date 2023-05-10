#include "Equalizer.h"

AS::IIRFilter::IIRFilter() {
}

AS::IIRFilter::~IIRFilter() {
}

void AS::IIRFilter::Process(float* _src, uint32_t _renderFrames) {
	for (uint32_t fram = 0; fram < _renderFrames; ++fram) {
		float dest = m_BiCoef.beta[0] / m_BiCoef.alpha[0] * _src[fram] +
			m_BiCoef.beta[1] / m_BiCoef.alpha[0] * m_IIRBuff.in[0] +
			m_BiCoef.beta[2] / m_BiCoef.alpha[0] * m_IIRBuff.in[1] -
			m_BiCoef.alpha[1] / m_BiCoef.alpha[0] * m_IIRBuff.out[0] -
			m_BiCoef.alpha[2] / m_BiCoef.alpha[0] * m_IIRBuff.out[1];

		m_IIRBuff.in[1] = m_IIRBuff.in[0];
		m_IIRBuff.in[0] = _src[fram];

		m_IIRBuff.out[1] = m_IIRBuff.out[0];
		m_IIRBuff.out[0] = dest;
	}
}

void AS::IIRFilter::Flush() {
	m_IIRBuff.Flush();
}

void AS::IIRFilter::LowPass(uint32_t _samplingFreq, float _cutoffFreq, float _q) {
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
void AS::IIRFilter::HighPass(uint32_t _samplingFreq, float _cutoffFreq, float _q) {
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
void AS::IIRFilter::BandPass(uint32_t _samplingFreq, float _centerFreq, float _bandwidth) {
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
void AS::IIRFilter::Notch(uint32_t _samplingFreq, float _centerFreq, float _bandwidth) {
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
void AS::IIRFilter::LowShelf(uint32_t _samplingFreq, float _cutoffFreq, float _q, float _gain) {
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
void AS::IIRFilter::HighShelf(uint32_t _samplingFreq, float _cutoffFreq, float _q, float _gain) {
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
void AS::IIRFilter::Peaking(uint32_t _samplingFreq, float _centerFreq, float _bandwidth, float _gain) {
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
void AS::IIRFilter::AllPass(uint32_t _samplingFreq, float _centerFreq, float _q) {
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