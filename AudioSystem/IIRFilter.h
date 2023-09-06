#ifndef _IIRFILTER_
#define _IIRFILTER_
#include "FilterBase.h"

namespace AS {
	enum class IIRFilterType {
		AS_IIRTYPE_LPF,
		AS_IIRTYPE_HPF,
		AS_IIRTYPE_BPF,
		AS_IIRTYPE_BEF,
		AS_IIRTYPE_LSF,
		AS_IIRTYPE_HSF,
		AS_IIRTYPE_PEAK,
		AS_IIRTYPE_APF
	};

	struct IIRLPFParam {
		IIRLPFParam() {}
		IIRLPFParam(int32_t _samplingFreq, double _cutoffFreq, double _q) :samplingFreq(_samplingFreq), cutoffFreq(_cutoffFreq), q(_q) {}
		int32_t samplingFreq = 0; double cutoffFreq = 0.0; double q = 0.0;
	};

	struct IIRHPFParam {
		IIRHPFParam() {}
		IIRHPFParam(int32_t _samplingFreq, double _cutoffFreq, double _q) :samplingFreq(_samplingFreq), cutoffFreq(_cutoffFreq), q(_q) {}
		int32_t samplingFreq = 0; double cutoffFreq = 0.0; double q = 0.0;
	};

	struct IIRBPFParam {
		IIRBPFParam() {}
		IIRBPFParam(int32_t _samplingFreq, double _centerFreq, double _bandwidth) :samplingFreq(_samplingFreq), centerFreq(_centerFreq), bandwidth(_bandwidth) {}
		int32_t samplingFreq = 0; double centerFreq = 0.0; double bandwidth = 0.0;
	};

	struct IIRBEFParam {
		IIRBEFParam() {}
		IIRBEFParam(int32_t _samplingFreq, double _centerFreq, double _bandwidth) :samplingFreq(_samplingFreq), centerFreq(_centerFreq), bandwidth(_bandwidth) {}
		int32_t samplingFreq = 0; double centerFreq = 0.0; double bandwidth = 0.0;
	};

	struct IIRLSFParam {
		IIRLSFParam() {}
		IIRLSFParam(int32_t _samplingFreq, double _cutoffFreq, double _q, double _gain) :samplingFreq(_samplingFreq), cutoffFreq(_cutoffFreq), q(_q), gain(_gain) {}
		int32_t samplingFreq = 0; double cutoffFreq = 0.0; double q = 0.0; double gain = 0.0;
	};

	struct IIRHSFParam {
		IIRHSFParam() {}
		IIRHSFParam(int32_t _samplingFreq, double _cutoffFreq, double _q, double _gain) :samplingFreq(_samplingFreq), cutoffFreq(_cutoffFreq), q(_q), gain(_gain) {}
		int32_t samplingFreq = 0; double cutoffFreq = 0.0; double q = 0.0; double gain = 0.0;
	};

	struct IIRPeakParam {
		IIRPeakParam() {}
		IIRPeakParam(int32_t _samplingFreq, double _centerFreq, double _bandwidth, double _gain) :samplingFreq(_samplingFreq), centerFreq(_centerFreq), bandwidth(_bandwidth), gain(_gain) {}
		int32_t samplingFreq = 0; double centerFreq = 0.0; double bandwidth = 0.0; double gain = 0.0;
	};

	struct IIRAPFParam {
		IIRAPFParam() {}
		IIRAPFParam(int32_t _samplingFreq, double _centerFreq, double _q) :samplingFreq(_samplingFreq), centerFreq(_centerFreq), q(_q) {}
		int32_t samplingFreq = 0; double centerFreq = 0.0; double q = 0.0;
	};

	struct IIRParam :public FilterParamBase {
		IIRParam() {};
		IIRParam(IIRFilterType _type, IIRLPFParam _param) :filterType(_type), lpfParam(_param) {};
		IIRParam(IIRFilterType _type, IIRHPFParam _param) :filterType(_type), hpfParam(_param) {};
		IIRParam(IIRFilterType _type, IIRBPFParam _param) :filterType(_type), bpfParam(_param) {};
		IIRParam(IIRFilterType _type, IIRBEFParam _param) :filterType(_type), befParam(_param) {};
		IIRParam(IIRFilterType _type, IIRLSFParam _param) :filterType(_type), lsfParam(_param) {};
		IIRParam(IIRFilterType _type, IIRHSFParam _param) :filterType(_type), hsfParam(_param) {};
		IIRParam(IIRFilterType _type, IIRPeakParam _param) :filterType(_type), peakParam(_param) {};
		IIRParam(IIRFilterType _type, IIRAPFParam _param) :filterType(_type), apfParam(_param) {};

		IIRFilterType filterType = IIRFilterType::AS_IIRTYPE_LPF;
		union {
			IIRLPFParam lpfParam;
			IIRHPFParam hpfParam;
			IIRBPFParam bpfParam;
			IIRBEFParam befParam;
			IIRLSFParam lsfParam;
			IIRHSFParam hsfParam;
			IIRPeakParam peakParam;
			IIRAPFParam apfParam;
		};
	};

	const int32_t cBuffers = 2;
	const int32_t cCoeficients = 3;
	class IIRFilter :public FilterBase {
	public:
		IIRFilter();
		~IIRFilter();

		void SetFilterParam(FilterParamBase& _filterParam)override;
		void Process(float* _src, int32_t _renderFrames)override;
		void Flush()override;

		void LowPass(int32_t _samplingFreq, double _cutoffFreq, double _q);
		void LowPass(IIRLPFParam& _param) { LowPass(_param.samplingFreq, _param.cutoffFreq, _param.q); }
		void HighPass(int32_t _samplingFreq, double _cutoffFreq, double _q);
		void HighPass(IIRHPFParam& _param) { HighPass(_param.samplingFreq, _param.cutoffFreq, _param.q); }
		void BandPass(int32_t _samplingFreq, double _centerFreq, double _bandwidth);
		void BandPass(IIRBPFParam& _param) { BandPass(_param.samplingFreq, _param.centerFreq, _param.bandwidth); }
		void BandEliminate(int32_t _samplingFreq, double _centerFreq, double _bandwidth);
		void BandEliminate(IIRBEFParam& _param) { BandEliminate(_param.samplingFreq, _param.centerFreq, _param.bandwidth); }
		void LowShelf(int32_t _samplingFreq, double _cutoffFreq, double _q, double _gain);
		void LowShelf(IIRLSFParam& _param) { LowShelf(_param.samplingFreq, _param.cutoffFreq, _param.q, _param.gain); }
		void HighShelf(int32_t _samplingFreq, double _cutoffFreq, double _q, double _gain);
		void HighShelf(IIRHSFParam& _param) { HighShelf(_param.samplingFreq, _param.cutoffFreq, _param.q, _param.gain); }
		void Peaking(int32_t _samplingFreq, double _centerFreq, double _bandwidth, double _gain);
		void Peaking(IIRPeakParam& _param) { Peaking(_param.samplingFreq, _param.centerFreq, _param.bandwidth, _param.gain); }
		void AllPass(int32_t _samplingFreq, double _centerFreq, double _q);
		void AllPass(IIRAPFParam& _param) { AllPass(_param.samplingFreq, _param.centerFreq, _param.q); }

	private:
		struct IIRBuffer {
			std::array<double, cBuffers> in{ 0.0, 0.0 };
			std::array<double, cBuffers> out{ 0.0, 0.0 };
			void Flush() { in = out = std::array<double, cBuffers>{0.0f, 0.0f}; }
		};
		struct BiquadCoefficient {
			BiquadCoefficient() {}
			BiquadCoefficient(double _alp0, double _alp1, double _alp2, double _bta0, double _bta1, double _bta2) {
				alpha = std::array<double, cCoeficients>{_alp0, _alp1, _alp2};
				beta = std::array<double, cCoeficients>{_bta0, _bta1, _bta2};
			}
			BiquadCoefficient(std::array<double, cCoeficients> _alpha, std::array<double, cCoeficients> _beta) :alpha(_alpha), beta(_beta) {
			}
			std::array<double, cCoeficients> alpha{ 0.0, 0.0, 0.0 };
			std::array<double, cCoeficients> beta{ 0.0, 0.0, 0.0 };
		};

		IIRBuffer m_IIRBuff;
		BiquadCoefficient m_BiCoef;
	};
}

#endif