#include "PCMNormalizer.h"

using namespace AS;
using namespace std;
using namespace myLib;

void PCMNormalizer::PCM_Normalize(byte* _pSrc, LineBuffer<double>& _dest, const AudioFormat _format, const int32_t _frames) {
	if (_frames > _dest.sizeX())return;

	std::function<void(byte*, double*)> func;
	uint16_t bdSize = _format.bitDepth().value() / 8;
	switch (_format.bitDepth().value()) {
	case 8:
		func = PCM_Normalize_8bit;
		break;
	case 16:
		func = PCM_Normalize_16bit;
		break;
	case 32:
		func = PCM_Normalize_32bit;
		break;
	}
	if (!func)return;

	for (uint16_t chan = 0; chan < _format.channel(); ++chan) {
		auto pSrc = _pSrc + (bdSize * chan);
		auto pDest = &_dest.at(chan).front();
		for (uint64_t fram = 0; fram < _frames; ++fram) {
			func(pSrc, pDest++);
			pSrc += (bdSize * _format.channel().value());
		}
	}
}

void PCMNormalizer::PCM_Denormalize(LineBuffer<double>& _src, byte* _pDest, const AudioFormat _format, const int32_t _frames) {
	if (_frames > _src.sizeX())return;

	std::function<void(double*, byte*)> func;
	uint16_t bdSize = _format.bitDepth().value() / 8;
	switch (_format.bitDepth().value()) {
	case 8:
		func = PCM_Denormalize_8bit;
		break;
	case 16:
		func = PCM_Denormalize_16bit;
		break;
	case 32:
		func = PCM_Denormalize_32bit;
		break;
	}
	if (!func)return;

	for (uint16_t chan = 0; chan < _format.channel().value(); ++chan) {
		auto pSrc = &_src.at(chan).front();
		auto pDest = _pDest + (bdSize * chan);
		for (uint64_t fram = 0; fram < _frames; ++fram) {
			func(pSrc++, pDest);
			pDest += (bdSize * _format.channel().value());
		}
	}
}

void PCMNormalizer::PCM_Normalize_8bit(byte* pSrc, double* pDest) {
}

void PCMNormalizer::PCM_Denormalize_8bit(double* pSrc, byte* pDest) {
}

void PCMNormalizer::PCM_Normalize_16bit(byte* pSrc, double* pDest) {
	int16_t raw = 0;
	memcpy(&raw, pSrc, 2);
	*pDest = static_cast<double>(raw) / static_cast<double>(0x8000);
}

void PCMNormalizer::PCM_Denormalize_16bit(double* pSrc, byte* pDest) {
	double src = *pSrc;
	src += 1.0;
	src = src / 2.0 * static_cast<double>(0x10000);
	if (src > static_cast<double>(0xffff))
		src = static_cast<double>(0xffff);
	else if (src < 0.0)
		src = 0.0;

	int16_t data = static_cast<int16_t>(static_cast<int32_t>(src + 0.5) - static_cast<int32_t>(0x8000));
	memcpy(pDest, &data, 2);
}