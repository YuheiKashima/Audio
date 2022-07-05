#include "AudioDefines.h"

void AS::PCMNormalizer::PCM_Normalize(byte* _pSrc, LineBuffer<float>& _dest, const uint32_t _destPoint, const AudioFormat _format, const uint32_t _frames) {
	if (_destPoint > _frames || _destPoint + _frames > _dest.sizeX())return;

	std::function<void(byte*, float*)> func;
	uint16_t bdSize = _format.bitDepth / BitsPerByte;
	switch (_format.bitDepth) {
	case 16:
		func = PCM_Normalize_16bit;
		break;
	}
	if (!func)return;

	auto pCopy = _pSrc;
	for (uint64_t fram = _destPoint; fram < _destPoint + _frames; ++fram) {
		for (uint16_t chan = 0; chan < _format.channnels; ++chan) {
			func(pCopy, &_dest[chan][fram]);
			pCopy += bdSize;
		}
	}
}

void AS::PCMNormalizer::PCM_Denormalize(LineBuffer<float>& _src, byte* _pDest, const uint32_t _destPoint, const AudioFormat _format, const uint32_t _frames) {
	if (_destPoint > _frames || _destPoint + _frames > _src.sizeX())return;

	std::function<void(float*, byte*)> func;
	uint16_t bdSize = _format.bitDepth / BitsPerByte;
	switch (_format.bitDepth) {
	case 16:
		func = PCM_Denormalize_16bit;
		break;
	}
	if (!func)return;

	auto pCopy = _pDest;
	for (uint64_t fram = _destPoint; fram < _destPoint + _frames; ++fram) {
		for (uint16_t chan = 0; chan < _format.channnels; ++chan) {
			func(&_src[chan][fram], pCopy);
			pCopy += bdSize;
		}
	}
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

uint32_t AS::TimeToFrames(const AudioFormat _format, const uint32_t _time) {
	return static_cast<uint32_t>(std::lroundf(_format.samplingRate * (_time / 1000.0f)));
}

uint32_t AS::FramesToTime(const AudioFormat _format, const uint32_t _frames) {
	return static_cast<uint32_t>(std::lroundf((_frames * 1000.0f) / _format.samplingRate));
}