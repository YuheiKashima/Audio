#include "MFourier.h"

std::function<double(uint32_t, size_t)> myLib::Fourier::m_sWindowFunction = myLib::Fourier::RectWindow;

myLib::FourierVariable myLib::Fourier::FFT(std::vector<double> _in, bool _inverse) {
	FourierVariable comp(std::bit_ceil(_in.size()));
	for (uint32_t i = 0; auto in:_in)comp[i] = std::complex<double>(m_sWindowFunction(i, _in.size()) * _in[i], 0);
	return FFT(comp, _inverse);
}

myLib::FourierVariable myLib::Fourier::FFT(FourierVariable _in, bool _inverse) {
	uint32_t n = _in.size();
	uint32_t h = 0; // h = log_2(n)
	const double _pi = std::numbers::pi;
	for (uint32_t i = 0; 1 << i < n; i++) h++;
	// バタフライ演算用の配置入れ替え
	for (uint32_t i = 0; i < n; i++) {
		uint32_t j = 0;
		for (uint32_t k = 0; k < h; k++) j |= (i >> k & 1) << (h - 1 - k);
		if (i < j) std::swap(_in[i], _in[j]);
	}

	//むげーんだーいなゆーめのーあとのー
	std::cout << "All blocks:" << n << std::endl;
	for (uint32_t b = 1; b < n; b *= 2) {
		std::cout << "Block:" << b << std::endl;
		//第log_2(blo) + 1段
		//blockSize = blo * 2
		for (uint32_t j = 0; j < b; j++) {
			std::cout << "\tProgress:" << std::floorf((static_cast<float>(j) / static_cast<float>(b)) * 10000.0f) / 100.0f << "\t" << j << "/" << b << std::endl;
			//ブロック内 j 個目
			//重み w = (1の原始 2b 乗根の j 乗)
			std::complex<double> w =
				std::polar(1.0, (2 * _pi) / (2 * b) * j * (_inverse ? 1 : -1));
			for (uint32_t k = 0; k < n; k += b * 2) {
				// k を先頭とするブロック
				std::complex<double> s = _in[j + k];			// 前
				std::complex<double> t = _in[j + k + b] * w;	// 後
				_in[j + k] = s + t;								// 前の更新
				_in[j + k + b] = s - t;							// 後の更新
			}
		}
	}

	std::cout << "Complete!!" << std::endl;

	//逆変換字にサイズで割る調整
	if (_inverse) {
		for (uint32_t i = 0; i < n; ++i)
			_in[i] /= n;
	}

	return _in;
}

void myLib::Fourier::OutputCSV(std::vector<FourierVariable> _output) {
	std::filesystem::create_directory("Fourier");
	std::ofstream file("Fourier/Fourier_" + Chrono::GetTime_str() + ".csv", std::ios::out);
	if (!file)return;

	std::stringstream strstr;
	for (uint32_t i = 0; auto & output : _output) {
		strstr << "Channel:" << i++ << std::endl;
		strstr << "Real" << std::endl;
		for (auto& out : output)
			strstr << out.real() << ",";
		strstr << std::endl;
		strstr << "Imag" << std::endl;
		for (auto& out : output) {
			strstr << out.imag() << ",";
		}
		strstr << std::endl;
	}
	file << strstr.str();
}