#include "FIRFilter.h"

AS::FIRFilter::FIRFilter() {
}

AS::FIRFilter::~FIRFilter() {
}

float AS::FIRFilter::Process(float _src) {
	//FIR
	float dest = 0.0f;

	return dest;
}

void AS::FIRFilter::Flush() {
}

/**
* LPF�̌W�����쐬����
* @param _samplingFreq �T���v�����O���g��
* @param _cutofFreq �J�b�g�I�t���g��
* @param _bandwidth �ш敝
*/
void AS::FIRFilter::LowPass(int32_t _samplingFreq, float _cutoffFreq, float _bandwidth) {
	//�G�b�W���g��
	float ef = _cutoffFreq / _samplingFreq;
	//�J�ڑш敝
	float delta = _bandwidth / _samplingFreq;
	//�x���@��
	m_CntCoefTaps = calctaps(delta);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_CntCoefTaps++;
	m_Coeficients = std::vector<float>(m_CntCoefTaps);

	for (auto i = 0; auto & coef : m_Coeficients) {
		//b(m) = 2 * fe * sinc(2 * pi * fe * m) (-inf <= m <= inf)
		coef = 2.0 * ef * sincf(2.0 * std::numbers::pi * ef * m(i));
		++i;
	}

	WindowFunc(m_Coeficients);
}

/**
* HPF�̌W�����쐬����
* @param _samplingFreq �T���v�����O���g��
* @param _cutofFreq �J�b�g�I�t���g��
* @param _bandwidth �ш敝
*/
void AS::FIRFilter::HighPass(int32_t _samplingFreq, float _cutoffFreq, float _bandwidth) {
	//�G�b�W���g��
	float ef = _cutoffFreq / _samplingFreq;
	//�J�ڑш敝
	float delta = _bandwidth / _samplingFreq;
	//�x���@��
	m_CntCoefTaps = calctaps(delta);

	auto m = [=](int32_t _i) {return  (_i - (m_CntCoefTaps - 1) / 2); };

	m_CntCoefTaps++;
	m_Coeficients = std::vector<float>(m_CntCoefTaps);

	for (int32_t i = 0; auto & coef:m_Coeficients) {
		//b(m) = sinc(pi * m) - 2 * fe * sinc(2 * pi * m) (-inf <= m <= inf)
		coef = sincf(std::numbers::pi * m(i)) - 2 * ef * sincf(2.0 * std::numbers::pi * ef * m(i));
		++i;
	}

	WindowFunc(m_Coeficients);
}

/**
* BPF�̌W�����쐬����
* @param _samplingFreq �T���v�����O���g��
* @param _cutofFreq1 �J�b�g�I�t���g��1
* @param _cutofFreq2 �J�b�g�I�t���g��2
* @param _bandwidth �ш敝
*/
void AS::FIRFilter::BandPass(int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth) {
	//�G�b�W���g��
	float ef1 = _cutoffFreq1 / _samplingFreq;
	float ef2 = _cutoffFreq2 / _samplingFreq;
	//�J�ڑш敝
	float delta = _bandwidth / _samplingFreq;
	//�x���@��
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

	WindowFunc(m_Coeficients);
}

void AS::FIRFilter::BandEliminate(int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth) {
	//�G�b�W���g��
	float ef1 = _cutoffFreq1 / _samplingFreq;
	float ef2 = _cutoffFreq2 / _samplingFreq;
	//�J�ڑш敝
	float delta = _bandwidth / _samplingFreq;
	//�x���@��
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
}

float AS::FIRFilter::sincf(float _x) {
	double y = 0.0;

	if (_x == 0.0) {
		y = 1.0;
	}
	else {
		y = sin(_x) / _x;
	}
	return y;
}

int32_t AS::FIRFilter::calctaps(float _delta) {
	int32_t d = std::round(3.1 / _delta) - 1;
	d = (d + 1) % 2 == 0 ? d++ : d;
	return d;
}

void AS::FIRFilter::SingbellWindow() {
}

void AS::FIRFilter::HanningWindow(std::vector<double>& _w) {
	if (_w.size() % 2 == 0) {
		for (auto i = 0; i < _w.size(); ++i) {
			_w[i] = 0.5 - 0.5 * cos(2.0 * std::numbers::pi * i / _w.size());
		}
	}
	else {
		for (auto i = 0; i < _w.size(); ++i) {
			_w[i] = 0.5 * 0.5 * cos(2.0 * std::numbers::pi * (i + 0.5) / _w.size());
		}
	}
}