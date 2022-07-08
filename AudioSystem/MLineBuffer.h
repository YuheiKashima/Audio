#ifndef _MLINEBUFFER_
#define _MLINEBUFFER_

#include "Utilities.h"

namespace myLib {
	//参考(というか写経)https://qiita.com/Gaccho/items/e936de237676120aa8a0
	template<typename T>
	class LineBuffer_XY {
	public:
		constexpr LineBuffer_XY<T>(const size_t _id, const std::unique_ptr<T[]>& _arr)
			: m_id_xy(_id), m_arr_ref(_arr) {}
		constexpr T operator+() const { return +m_arr_ref[m_id_xy]; }
		constexpr T operator-() const { return -m_arr_ref[m_id_xy]; }
		constexpr T operator+(const T& _a) const { return m_arr_ref[m_id_xy] + _a; }
		constexpr T operator-(const T& _a) const { return m_arr_ref[m_id_xy] - _a; }
		constexpr T operator*(const T& _a) const { return m_arr_ref[m_id_xy] * _a; }
		constexpr T operator/(const T& _a) const { return m_arr_ref[m_id_xy] / _a; }
		constexpr T operator%(const T& _a) const { return m_arr_ref[m_id_xy] % _a; }
		constexpr T operator&(const T& _a) const { return m_arr_ref[m_id_xy] & _a; }
		constexpr T operator|(const T& _a) const { return m_arr_ref[m_id_xy] | _a; }
		constexpr T operator^(const T& _a) const { return m_arr_ref[m_id_xy] ^ _a; }
		constexpr T operator=(const T& _a) const { return m_arr_ref[m_id_xy] = _a; }
		constexpr T operator+=(const T& _a) const { return m_arr_ref[m_id_xy] += _a; }
		constexpr T operator-=(const T& _a) const { return m_arr_ref[m_id_xy] -= _a; }
		constexpr T operator*=(const T& _a) const { return m_arr_ref[m_id_xy] *= _a; }
		constexpr T operator/=(const T& _a) const { return m_arr_ref[m_id_xy] /= _a; }
		constexpr T operator%=(const T& _a) const { return m_arr_ref[m_id_xy] %= _a; }
		constexpr T operator^=(const T& _a) const { return m_arr_ref[m_id_xy] ^= _a; }
		constexpr T operator|=(const T& _a) const { return m_arr_ref[m_id_xy] |= _a; }
		constexpr T operator&=(const T& _a) const { return m_arr_ref[m_id_xy] &= _a; }
		constexpr T operator++() const { return ++m_arr_ref[m_id_xy]; }
		constexpr T operator++(int) const { return m_arr_ref[m_id_xy]++; }
		constexpr T operator--() const { return --m_arr_ref[m_id_xy]; }
		constexpr T operator--(int) const { return m_arr_ref[m_id_xy]--; }
		constexpr T* operator&()const { return &m_arr_ref[m_id_xy]; }
		constexpr const bool operator!() const { return !m_arr_ref[m_id_xy]; }
		constexpr const bool operator&&(const T& _a) const { return (m_arr_ref[m_id_xy] && _a); }
		constexpr const bool operator||(const T& _a) const { return (m_arr_ref[m_id_xy] || _a); }
		constexpr const bool operator<(const T& _a) const { return (m_arr_ref[m_id_xy] < _a); }
		constexpr const bool operator>(const T& _a) const { return (m_arr_ref[m_id_xy] > _a); }
		constexpr const bool operator<=(const T& _a) const { return (m_arr_ref[m_id_xy] <= _a); }
		constexpr const bool operator>=(const T& _a) const { return (m_arr_ref[m_id_xy] >= _a); }
		constexpr const bool operator!=(const T& _a) const { return (m_arr_ref[m_id_xy] != _a); }
		constexpr const bool operator==(const T& _a) const { return (m_arr_ref[m_id_xy] == _a); }

		operator T() const { return m_arr_ref[m_id_xy]; }

	private:
		//ここに2次元配列最終的な配列番号に見立てた1次元配列の座標が入る
		const size_t m_id_xy = 0;
		//1次元配列の参照(LineBuffer.tの参照)
		const std::unique_ptr<T[]>& m_arr_ref;
	};

	template<typename T>
	class LineBuffer_X {
	public:
		constexpr LineBuffer_X<T>(const size_t _id, const std::unique_ptr<T[]>& _arr) : m_id_x(_id), m_arr_ref(_arr) {}
		//(2つ目の[])
		constexpr const LineBuffer_XY<T> operator[](const size_t _x) const {
			const LineBuffer_XY<T> arr_xy(m_id_x + _x, m_arr_ref);
			return arr_xy;
		}
		constexpr const LineBuffer_XY<T> at(const size_t _x) const {
			const LineBuffer_XY<T> arr_xy(m_id_x + _x, m_arr_ref);
			return arr_xy;
		}
		constexpr const LineBuffer_XY<T> front() const {
			const LineBuffer_XY<T> arr_xy(m_id_x, m_arr_ref);
			return arr_xy;
		}
	private:
		//ここには2次元配列のx軸先頭に見立てた1次元配列の座標が入る
		const size_t m_id_x = 0;
		//1次元配列の参照(LineBuffer.tの参照)
		const std::unique_ptr<T[]>& m_arr_ref;
	};

	template<typename T>
	class LineBuffer {
	public:
		LineBuffer<T>() {}
		//x*yの1次元配列を生成してすべての配列をvalueにする
		LineBuffer<T>(const size_t _y, const size_t _x, const T _value) {
			create(_y, _x);
			zeroclear();
			std::memset(m_arr_real.get(), _value, sizeof(T) * capasity());
		}

		//x*yの1次元配列を生成するだけ
		constexpr LineBuffer<T>(const size_t _y, const size_t _x) {
			create(_y, _x);
			zeroclear();
		}

		//2次元配列に見立てた1次元配列のx軸の先頭のLineBuffer_Xを入手(1つ目の[])
		constexpr const LineBuffer_X<T> operator[](const size_t _y) const {
			LineBuffer_X<T> arr_x(_y * capasityX(), m_arr_real);
			return arr_x;
		}
		constexpr const LineBuffer<T>& operator =(const LineBuffer<T>& _src) {
			if (capasity() != _src.capasity()) {
				m_arr_real.reset();
				create(_src.m_Size_y, _src.m_Size_x);
				zeroclear();
			}
			memcpy_s(m_arr_real.get(), sizeof(T) * capasity(), _src.m_arr_real.get(), sizeof(T) * capasity());
			return *this;
		}
		constexpr T* operator&() const {
			return m_arr_real.get();
		}

		constexpr const LineBuffer_X<T> at(const size_t _y) const {
			return LineBuffer_X(_y * capasityX(), m_arr_real);
		}
		constexpr const LineBuffer_X<T> front() const {
			return LineBuffer_X(0, m_arr_real);
		}
		constexpr size_t size() const {
			return m_Size_x * m_Size_y;
		}
		constexpr size_t capasity() const {
			return m_Capasity_x * m_Size_y;
		}
		constexpr size_t sizeX() const {
			return m_Size_x;
		}
		constexpr size_t capasityX() const {
			return m_Capasity_x;
		}
		constexpr size_t sizeY() const {
			return m_Size_y;
		}
		constexpr T* const get() const {
			return m_arr_real.get();
		}

		void resize(const size_t _y, const size_t _x) {
			m_arr_real.reset();
			create(_y, _x);
			zeroclear();
		}
		void zeroclear() {
			std::memset(m_arr_real.get(), NULL, sizeof(T) * capasity());
		}
		std::unique_ptr<T[]> data() const;
		bool empty() {
			return m_arr_real ? false : true;
		}
		void swap(LineBuffer<T>& _swap) {
			size_t tempx = m_Size_x, tempy = m_Size_y, tempRx = m_Capasity_x;
			m_Size_x = _swap.m_Size_x;
			m_Size_y = _swap.m_Size_y;
			m_Capasity_x = _swap.m_Capasity_x;

			_swap.m_Size_x = tempx;
			_swap.m_Size_y = tempy;
			_swap.m_Capasity_x = tempRx;

			std::swap(m_arr_real, _swap.m_arr_real);
		}

		void avx_add(LineBuffer<float>& _add) {
			const auto processFrames = std::min(this->capasityX(), _add.capasityX());
			const auto avxForAdd = sizeof(__m256) / sizeof(float);

			for (uint32_t i = 0; i < sizeY(); ++i) {
				float* ptrThis = &at(i).front();
				float* ptrAdd = &_add.at(i).front();
				for (uint32_t j = 0; j < processFrames; j += avxForAdd) {
					__m256 avxThis = _mm256_load_ps(ptrThis);
					__m256 avxAdd = _mm256_load_ps(ptrAdd);
					_mm256_store_ps(ptrThis, _mm256_add_ps(avxThis, avxAdd));
					ptrThis += avxForAdd;
					ptrAdd += avxForAdd;
				}
			}
		}
		void avx_mul(float _mul) {
			const uint32_t avxForAdd = sizeof(__m256) / sizeof(float);
			const __m256 mul = _mm256_set_ps(_mul, _mul, _mul, _mul, _mul, _mul, _mul, _mul);

			auto ptrThis = this->get();
			for (uint32_t i = 0; i < this->size(); i += avxForAdd) {
				__m256 avxThis = _mm256_load_ps(ptrThis);
				_mm256_store_ps(ptrThis, _mm256_mul_ps(avxThis, mul));

				ptrThis += avxForAdd;
			}
		}

	private:
		std::unique_ptr<T[]> m_arr_real;
		size_t m_Size_x = 0, m_Size_y = 0, m_Capasity_x = 0;

		void create(const size_t _y, const size_t _x) {
			m_Size_x = _x == 0 ? 1 : _x;
			m_Size_y = _y == 0 ? 1 : _y;
			m_Capasity_x = calcAlienRange(m_Size_x);
			m_arr_real = std::make_unique<T[]>(m_Capasity_x * m_Size_y);
		}
	};
};
//要素番号探索式((y*m_Size_x)+x)

#endif