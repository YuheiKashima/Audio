#include "Resampler.h"

AS::Resampler::Resampler() {
}

AS::Resampler::~Resampler() {
}

uint32_t AS::Resampler::CalcSamples(uint32_t _freqFrom, uint32_t _cntSammp, uint32_t _freqTo) {
	if (_freqTo > _freqFrom) {
		return _cntSammp / (_freqTo / _freqFrom);
	}
	else if (_freqFrom > _freqTo) {
		return _cntSammp * (_freqFrom / _freqTo);
	}
	return 0;
}

uint32_t AS::Resampler::Resampling(LineBuffer<float>& _src, uint32_t _freqFrom, uint32_t _freqTo, LineBuffer<float>& _dest) {
	double reSampRate = (_freqTo / _freqFrom);
	double nyquistFreq = _freqTo > _freqFrom ? _freqFrom / 2.0 : _freqTo / 2.0;
	uint32_t dest = 0;

	if (_freqTo > _freqFrom) {
		dest = UpSampling(_src, reSampRate, _dest);
	}

	//アップサンプリングは0補完後変換前ナイキスト周波数をカットオフとするローパスフィルタを通す
	//ダウンサンプリングは間引き前に変換後ナイキスト周波数をカットオフとするローパスフィルタを通す
	m_lpf.LowPass(FIRWindow::FIR_WINDOW_HANNING, _freqTo, nyquistFreq, 1.0);
	for (int32_t chan = 0; chan < _dest.sizeY(); ++chan) {
		m_lpf.Process(&_dest.at(chan).front(), _dest.sizeX());
	}

	if (_freqFrom > _freqTo) {
		dest = DownSampling(_src, reSampRate, _dest);
	}

	return dest;
}

uint32_t AS::Resampler::UpSampling(LineBuffer<float>& _src, double _resampRate, LineBuffer<float>& _dest) {
	double upsampRate = _resampRate - 1;
	_dest = LineBuffer<float>(_src.sizeX(), _src.sizeY() * _resampRate);

	for (uint32_t chan = 0; chan < _dest.sizeY(); ++chan) {
		auto pDest = &_dest.at(chan).front();
		auto pSrc = &_src.at(chan).front();
		double counter = 0.0;

		for (uint32_t frams = 0; frams < _dest.sizeX(); ++frams) {
			if (counter >= 1.0) {
				*pDest = 0.0;
				counter -= 1.0;
			}
			else {
				*pDest = *pSrc;
				++pSrc;
			}
			++pDest;
			counter += upsampRate;
		}
	}
	return _dest.size();
}

uint32_t AS::Resampler::DownSampling(LineBuffer<float>& _src, double _resampRate, LineBuffer<float>& _dest) {
	double upsampRate = _resampRate - 1;
	_dest = LineBuffer<float>(_src.sizeX(), _src.sizeY() * _resampRate);

	for (uint32_t chan = 0; chan < _dest.sizeY(); ++chan) {
		auto pDest = &_dest.at(chan).front();
		auto pSrc = &_src.at(chan).front();
		double counter = 0.0;

		for (uint32_t frams = 0; frams < _dest.sizeX(); ++frams) {
			if (counter >= 1.0) {
				counter -= 1.0;
			}
			else {
				*pDest = *pSrc;
				++pDest;
			}
			++pSrc;
			counter += upsampRate;
		}
	}
	return _dest.size();
}