#include "AudioDefines.h"

void AS::PCMNormalizer::PCM_Normalize(byte* _pSrc, LineBuffer<float>& _dest, const AudioFormat _format, const uint32_t _frames) {
	if (_frames > _dest.sizeX())return;

	std::function<void(byte*, float*)> func;
	uint16_t bdSize = _format.bitDepth / BitsPerByte;
	switch (_format.bitDepth) {
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

	for (uint16_t chan = 0; chan < _format.channnels; ++chan) {
		auto pSrc = _pSrc + (bdSize * chan);
		auto pDest = &_dest.at(chan).front();
		for (uint64_t fram = 0; fram < _frames; ++fram) {
			func(pSrc, pDest++);
			pSrc += (bdSize * _format.channnels);
		}
	}
}

void AS::PCMNormalizer::PCM_Denormalize(LineBuffer<float>& _src, byte* _pDest, const AudioFormat _format, const uint32_t _frames) {
	if (_frames > _src.sizeX())return;

	std::function<void(float*, byte*)> func;
	uint16_t bdSize = _format.bitDepth / BitsPerByte;
	switch (_format.bitDepth) {
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

	for (uint16_t chan = 0; chan < _format.channnels; ++chan) {
		auto pSrc = &_src.at(chan).front();
		auto pDest = _pDest + (bdSize * chan);
		for (uint64_t fram = 0; fram < _frames; ++fram) {
			func(pSrc++, pDest);
			pDest += (bdSize * _format.channnels);
		}
	}
}

void AS::PCMNormalizer::PCM_Normalize_8bit(byte* pSrc, float* pDest) {
}

void AS::PCMNormalizer::PCM_Denormalize_8bit(float* pSrc, byte* pDest) {
}

void AS::PCMNormalizer::PCM_Normalize_16bit(byte* pSrc, float* pDest) {
	int16_t raw = 0;
	memcpy(&raw, pSrc, 2);
	*pDest = static_cast<float>(raw) / static_cast<float>(0x8000);
}

void AS::PCMNormalizer::PCM_Denormalize_16bit(float* pSrc, byte* pDest) {
	float src = *pSrc;
	src += 1.0f;
	src = src / 2.0f * static_cast<float>(0x10000);
	if (src > static_cast<float>(0xffff))
		src = static_cast<float>(0xffff);
	else if (src < 0.0f)
		src = 0.0f;

	int16_t data = static_cast<int16_t>(static_cast<int32_t>(src + 0.5) - static_cast<int32_t>(0x8000));
	memcpy(pDest, &data, 2);
}

void AS::PCMNormalizer::PCM_Normalize_32bit(byte* pSrc, float* pDest) {
}

void AS::PCMNormalizer::PCM_Denormalize_32bit(float* pSrc, byte* pDest) {
}

uint32_t AS::TimeToFrames(const AudioFormat _format, const uint32_t _time) {
	return static_cast<uint32_t>(std::lroundf(_format.samplingRate * (_time / 1000.0f)));
}

uint32_t AS::FramesToTime(const AudioFormat _format, const uint32_t _frames) {
	return static_cast<uint32_t>(std::lroundf((_frames * 1000.0f) / _format.samplingRate));
}