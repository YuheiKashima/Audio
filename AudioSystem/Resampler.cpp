#include "Resampler.h"

AS::Resampler::Resampler() {
}

AS::Resampler::~Resampler() {
}

uint32_t AS::Resampler::CalcSamples(uint32_t _freqFrom, uint32_t _cntSammp, uint32_t _freqTo) {
	int32_t resampRate = (std::max(_freqTo, _freqFrom) / std::min(_freqTo, _freqFrom));
	return _freqTo > _freqFrom ? _cntSammp / resampRate : _freqFrom > _freqTo ? _cntSammp * resampRate : 0;
}

uint32_t AS::Resampler::Resampling(LineBuffer<float>& _src, uint32_t _freqFrom, uint32_t _freqTo, LineBuffer<float>& _dest) {
	int32_t reSampRate = (std::max(_freqTo, _freqFrom) / std::min(_freqTo, _freqFrom));
	double nyquistFreq = std::min(_freqFrom, _freqTo) / 2.0;
	uint32_t dest = 0;

	if (_freqTo > _freqFrom) {
		dest = Expansion(_src, reSampRate, _dest);
	}

	//アップサンプリングは0補完後変換前ナイキスト周波数をカットオフとするローパスフィルタを通す
	//ダウンサンプリングは間引き前に変換後ナイキスト周波数をカットオフとするローパスフィルタを通す
	m_lpf.LowPass(FIRWindow::FIR_WINDOW_HANNING, _freqTo, nyquistFreq, 1.0);
	for (int32_t chan = 0; chan < _dest.sizeY(); ++chan) {
		m_lpf.Process(&_dest.at(chan).front(), _dest.sizeX());
	}

	if (_freqFrom > _freqTo) {
		dest = Decimation(_src, reSampRate, _dest);
	}

	return dest;
}

uint32_t AS::Resampler::Expansion(LineBuffer<float>& _src, double _resampRate, LineBuffer<float>& _dest) {
	int32_t upsampRate = _resampRate - 1;
	_dest = LineBuffer<float>(_src.sizeX(), _src.sizeY() * _resampRate);
	_dest.zeroclear();

	for (uint32_t chan = 0; chan < _src.sizeY(); ++chan) {
		auto pDest = &_dest.at(chan).front();
		auto pSrc = &_src.at(chan).front();

		for (uint32_t frams = 0; frams < _src.sizeX(); ++frams) {
			*pDest = *pSrc;
			pDest += upsampRate;
			++pSrc;
		}
	}
	return _dest.size();
}

uint32_t AS::Resampler::Decimation(LineBuffer<float>& _src, double _resampRate, LineBuffer<float>& _dest) {
	int32_t upsampRate = _resampRate - 1;
	_dest = LineBuffer<float>(_src.sizeX(), _src.sizeY() / _resampRate);
	_dest.zeroclear();

	for (uint32_t chan = 0; chan < _dest.sizeY(); ++chan) {
		auto pDest = &_dest.at(chan).front();
		auto pSrc = &_src.at(chan).front();

		for (uint32_t frams = 0; frams < _dest.sizeX(); ++frams) {
			*pDest = *pSrc;
			pSrc += upsampRate;
			++pDest;
		}
	}
	return _dest.size();
}