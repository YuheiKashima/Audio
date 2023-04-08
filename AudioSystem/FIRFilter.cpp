#include "FIRFilter.h"

void FIRDemo(double* _pSrc, double* _pDest, int32_t _length, double _sampFreq, double _bitDepth) {
	//エッジ周波数
	double edgeFreq = 1000.0 / _sampFreq;//(cutoffFreq / sampFreq)
	//遷移帯域幅
	double delta = 1000.0 / _sampFreq;//(QFreq / sampFreq)Q値に価するもの?
	//遅延器数
	int32_t delayDevice = static_cast<int32_t>(3.1 / delta + 0.5) - 1;
	//delayDevice+1が奇数になるよう調整
	delayDevice % 2 == 1 ? delayDevice + 1 : delayDevice;

	//フィルタ係数の設定
	std::vector<double> b(delayDevice + 1);
	std::vector<double> w(delayDevice + 1);

	//HanningWindow

	//FIR
	for (auto i = 0; i < _length; ++i) {
		for (auto j = 0; j <= delayDevice; ++j) {
			if (i - j >= 0) {
				_pDest[i] += b[j] * _pSrc[i - j];
			}
		}
	}
}

namespace WINDOW_FUNC {
	void Hanning_Window(std::vector<double>& _w) {
		if (_w.size() % 2 == 0) {
			for (auto i = 0; i < _w.size(); ++i) {
				_w[i] = 0.5 - 0.5 * cos(2.0 * std::numbers::pi * i / _w.size());
			}
		}
		else {
			for (auto i = 0; i < _w.size(); ++i) {
				_w[i] = 0.5 * 0.5 * cos(2.0 * std::numbers::pi * (i + 0.5) / _w.size());
			}
		}
	}
}

namespace FIR {
	double sinc(double _x) {
		double y = 0.0;

		if (_x == 0.0) {
			y = 1.0;
		}
		else {
			y = sin(_x) / _x;
		}
		return y;
	}

	void LPF(double _fe, int32_t _j, std::vector<double>& _b, std::vector<double>& _w) {
		int32_t offset = _j / 2;

		//b(m) = 2 * fe * sinc(2 * pi * fe * m) (-inf <= m <= inf)
		for (auto m = -_j / 2; m <= _j / 2; ++m) {
			_b[offset + m] = 2.0 * _fe * sinc(2.0 * std::numbers::pi * _fe * m);
		}

		for (auto m = 0; m < _j + 1; ++m) {
			_b[m] *= _w[m];
		}
	}

	void HPF(double _fe, int32_t _j, std::vector<double>& _b, std::vector<double>& _w) {
		int32_t offset = _j / 2;

		//b(m) = sinc(pi * m)-2 * fe * sinc(2 * pi * m) (-inf <= m <= inf)
		for (auto m = -_j / 2; m <= _j / 2; ++m) {
			_b[offset + m] = sinc(std::numbers::pi * m) - 2.0 * _fe * sinc(2.0 * std::numbers::pi * _fe * m);
		}

		for (auto m = 0; m < _j + 1; ++m) {
			_b[m] *= _w[m];
		}
	}

	void BPF(double _fe1, double _fe2, int32_t _j, std::vector<double>& _b, std::vector<double>& _w) {
		int32_t offset = _j / 2;

		//b(m) = 2 * fe2 *sinc(2 * pi * fe2 * m) - 2 * fe1 * sinc(2 * pi * fe1 * m) (-inf <= m <= inf)
		for (auto m = -_j / 2; m <= _j / 2; ++m) {
			_b[offset + m] =
				2.0 * _fe2 * sinc(2.0 * std::numbers::pi * _fe2 * m)
				- 2.0 * _fe1 * sinc(2.0 * std::numbers::pi * _fe1 * m);
		}

		for (auto m = 0; m < _j + 1; ++m) {
			_b[m] *= _w[m];
		}
	}

	void BEF(double _fe1, double _fe2, int32_t _j, std::vector<double>& b, std::vector<double> _w) {
		int32_t offset = _j / 2;

		//b(m) = sinc(pi * m) - 2 * fe2 * sinc(2 * pi *fe2 * m) + 2 * fe1 * sinc(2 * pi *fe1 * m) (-inf <= m <= inf)
		for (auto m = -_j / 2; m <= _j / 2; ++m) {
			_b[offset + m] = sinc(std::numbers::pi * m)
				- 2.0 * _fe2 * sinc(2.0 * std::numbers::pi * _fe2 * m)
				+ 2.0 * _fe1 * sinc(2.0 * std::numbers::pi * _fe1 * m);
		}

		for (auto m = 0; m < _j + 1; ++m) {
			_b[m] *= _w[m];
		}
	}
}