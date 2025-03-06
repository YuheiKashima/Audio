/**

	@file      PCMNormalizer.h
	@brief
	@details   ~
	@author    Yuhei kashima
	@date      15.02.2025

**/

#ifndef _PCMNORMALIZER_
#define _PCMNORMALIZER_

#include <functional>

#include "AudioDefine.h"
#include "AudioFormat.h"

namespace AS {
	/**

		@class   PCMNormalizer
		@brief
		@details ~

	**/
	class PCMNormalizer {
	public:
		static void PCM_Normalize(byte* _pSrc, myLib::LineBuffer<double>& _dest, const AudioFormat _format, const int32_t _frames);
		static void PCM_Denormalize(myLib::LineBuffer<double>& _src, byte* _pDest, const AudioFormat _format, const int32_t _frames);

	private:
		static void PCM_Normalize_8bit(byte* _pSrc, double* _pDest);
		static void PCM_Denormalize_8bit(double* _pSrc, byte* _pDest);
		static void PCM_Normalize_16bit(byte* _pSrc, double* _pDest);
		static void PCM_Denormalize_16bit(double* _pSrc, byte* _pDest);
		static void PCM_Normalize_32bit(byte* _pSrc, double* _pDest);
		static void PCM_Denormalize_32bit(double* _pSrc, byte* _pDest);
	};
}

#endif // !_PCMNORMALIZER_