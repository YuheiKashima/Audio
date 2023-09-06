#ifndef _FILTERBASE_
#define _FILTERBASE_

#include "AudioDefines.h"

namespace AS {
	enum class WindowFuncType {
		AS_WINDOWFUNC_RECTANGULAR,
		AS_WINDOWFUNC_SINGBELL,
		AS_WINDOWFUNC_HANNING,
		AS_WINDOWFUNC_HAMMING,
		AS_WINDOWFUNC_BLACKMAN
	};

	struct FilterParamBase {};

	class FilterBase {
	public:
		FilterBase() {};
		virtual ~FilterBase() {};

		virtual void SetFilterParam(FilterParamBase& _filterParam) = 0;
		virtual void Process(float* _pSrc, int32_t _effectFrames) = 0;
		virtual void Flush() = 0;
	protected:
		void CreateWindowFunction(WindowFuncType _type, std::vector<double>& _destWindowCoeff);
		void CreateWindowFuncMultiplyCoeffitient(std::vector<double>& _coef, WindowFuncType _type);

		std::vector<double> m_WindowFuncCoefficients;
		WindowFuncType m_WindowSelector = WindowFuncType::AS_WINDOWFUNC_RECTANGULAR;
	private:
		void RecctangularWindow(std::vector<double>& _coefs);
		void SingbellWindow(std::vector<double>& _coefs);
		void HanningWindow(std::vector<double>& _coefs);
		void HammingWindow(std::vector<double>& _coefs);
		void BlackmanWindow(std::vector<double>& _coefs);
	};
}
#endif