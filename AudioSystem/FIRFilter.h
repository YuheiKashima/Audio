#ifndef _FIRFILTER_
#define _FIRFILTER_

#include "FilterBase.h"

//éQçl
//https://github.com/T-Sumida/FIRFilter
//

namespace AS {
	enum class FIRFilterType {
		AS_FIRTYPE_LPF,
		AS_FIRTYPE_HPF,
		AS_FIRTYPE_BPF,
		AS_FIRTYPE_BEF
	};

	struct FIRLPFParam {
		FIRLPFParam() { };
		FIRLPFParam(int32_t _samplingFreq, float _cutoffFreq, float _bandwidth) :samplingFreq(_samplingFreq), cutoffFreq(_cutoffFreq), bandwidth(_bandwidth) {}
		int32_t samplingFreq = 0; float cutoffFreq = 0.0f; float bandwidth = 0.0f;
	};

	struct FIRHPFParam {
		FIRHPFParam() {};
		FIRHPFParam(int32_t _samplingFreq, float _cutoffFreq, float _bandwidth) :samplingFreq(_samplingFreq), cutoffFreq(_cutoffFreq), bandwidth(_bandwidth) {}
		int32_t samplingFreq = 0; float cutoffFreq = 0.0f; float bandwidth = 0.0f;
	};

	struct FIRBPFParam {
		FIRBPFParam() {};
		FIRBPFParam(int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth) :samplingFreq(_samplingFreq), cutoffFreq1(_cutoffFreq1), cutoffFreq2(_cutoffFreq2), bandwidth(_bandwidth) {}
		int32_t samplingFreq = 0; float cutoffFreq1 = 0.0f; float cutoffFreq2 = 0.0f; float bandwidth = 0.0f;
	};

	struct FIRBEFParam {
		FIRBEFParam() {};
		FIRBEFParam(int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth) :samplingFreq(_samplingFreq), cutoffFreq1(_cutoffFreq1), cutoffFreq2(_cutoffFreq2), bandwidth(_bandwidth) {}
		int32_t samplingFreq = 0; float cutoffFreq1 = 0.0f; float cutoffFreq2 = 0.0f; float bandwidth = 0.0f;
	};

	struct FIRParam :public FilterParamBase {
		FIRParam() {};
		FIRParam(FIRFilterType _type, FIRLPFParam _param, WindowFuncType _window) :filterType(_type), lpfParam(_param), windowType(_window) {};
		FIRParam(FIRFilterType _type, FIRHPFParam _param, WindowFuncType _window) :filterType(_type), hpfParam(_param), windowType(_window) {};
		FIRParam(FIRFilterType _type, FIRBPFParam _param, WindowFuncType _window) :filterType(_type), bpfParam(_param), windowType(_window) {};
		FIRParam(FIRFilterType _type, FIRBEFParam _param, WindowFuncType _window) :filterType(_type), befParam(_param), windowType(_window) {};

		FIRFilterType filterType = FIRFilterType::AS_FIRTYPE_LPF;
		WindowFuncType windowType = WindowFuncType::AS_WINDOWFUNC_RECTANGULAR;
		union {
			FIRLPFParam lpfParam;
			FIRHPFParam hpfParam;
			FIRBPFParam bpfParam;
			FIRBEFParam befParam;
		};
	};

	class FIRFilter :public FilterBase {
	public:
		FIRFilter();
		~FIRFilter();

		void SetFilterParam(FilterParamBase& _filterParam)override;
		void Process(float* _src, int32_t _renderFrames)override;
		void Flush()override;

		void ChangeWindowFunc(WindowFuncType _window);

		void LowPass(int32_t _samplingFreq, float _cutoffFreq, float _bandwidth);
		void LowPass(FIRLPFParam& _param) { LowPass(_param.samplingFreq, _param.cutoffFreq, _param.bandwidth); }
		void HighPass(int32_t _samplingFreq, float _cutoffFreq, float _bandwidth);
		void HighPass(FIRHPFParam& _param) { HighPass(_param.samplingFreq, _param.cutoffFreq, _param.bandwidth); }
		void BandPass(int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth);
		void BandPass(FIRBPFParam& _param) { BandPass(_param.samplingFreq, _param.cutoffFreq1, _param.cutoffFreq2, _param.bandwidth); }
		void BandEliminate(int32_t _samplingFreq, float _cutoffFreq1, float _cutoffFreq2, float _bandwidth);
		void BandEliminate(FIRBEFParam& _param) { BandEliminate(_param.samplingFreq, _param.cutoffFreq1, _param.cutoffFreq2, _param.bandwidth); }

	private:
		double sinc(double _x);
		int32_t calctaps(double _delta);

		int32_t m_CntCoefTaps = 0;
		std::vector<double> m_FIRCoefficients;

		boost::circular_buffer<double> m_FIRFeedbackBuffer;
	};
}
#endif