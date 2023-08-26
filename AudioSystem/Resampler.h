#ifndef _RESAMPLER_
#define _RESAMPLER_

#include "FIRFilter.h"

//����͐����{�A��������1��1��ϊ����郊�T���v���ɂȂ��Ă���B
//���S�ȃ��T���v��(44.1->48�ϊ��Ƃ��ł���)���쐬����ɂ͉��L�̂悤�Ȃ��̂����K�v������B
//
//http://www.kumikomi.net/fpga/sample/0011/FPGA11_008.pdf
//
//https://ja.wikipedia.org/wiki/%E3%82%B5%E3%83%B3%E3%83%97%E3%83%AA%E3%83%B3%E3%82%B0%E5%91%A8%E6%B3%A2%E6%95%B0%E5%A4%89%E6%8F%9B

namespace AS {
	class Resampler {
	public:
		Resampler();
		~Resampler();
		static int32_t CalcSamples(int32_t _freqFrom, int32_t _cntSammp, int32_t _freqTo);
		int32_t Resampling(LineBuffer<float>& _src, int32_t _freqFrom, int32_t _freqTo, LineBuffer<float>& _dest);
		int32_t FixResamp441to480(LineBuffer<float>& _src, LineBuffer<float>& _dest);
		int32_t FixResamp480to441(LineBuffer<float>& _src, LineBuffer<float>& _dest);
	private:
		FIRFilter m_lpf;

		int32_t Expansion(LineBuffer<float>& _src, double _resampRate, LineBuffer<float>& _dest);
		int32_t Decimation(LineBuffer<float>& _src, double _resampRate, LineBuffer<float>& _dest);
	};
}
#endif