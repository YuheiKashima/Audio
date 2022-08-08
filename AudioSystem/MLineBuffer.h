#ifndef _MLINEBUFFER_
#define _MLINEBUFFER_

#include "Utilities.h"
#include "CPUSupportChecker.h"

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
			std::memset(m_arr_real.get(), _value, sizeof(T) * capacity());
		}

		//x*yの1次元配列を生成するだけ
		constexpr LineBuffer<T>(const size_t _y, const size_t _x) {
			create(_y, _x);
			zeroclear();
		}

		//2次元配列に見立てた1次元配列のx軸の先頭のLineBuffer_Xを入手(1つ目の[])
		constexpr const LineBuffer_X<T> operator[](const size_t _y) const {
			LineBuffer_X<T> arr_x(_y * capacityX(), m_arr_real);
			return arr_x;
		}
		constexpr const LineBuffer<T>& operator =(const LineBuffer<T>& _src) {
			if (capacity() != _src.capacity()) {
				m_arr_real.reset();
				create(_src.m_Size_y, _src.m_Size_x);
				zeroclear();
			}
			memcpy_s(m_arr_real.get(), sizeof(T) * capacity(), _src.m_arr_real.get(), sizeof(T) * capacity());
			return *this;
		}
		constexpr T* operator&() const {
			return m_arr_real.get();
		}

		constexpr const LineBuffer_X<T> at(const size_t _y) const {
			return LineBuffer_X(_y * capacityX(), m_arr_real);
		}
		constexpr const LineBuffer_X<T> front() const {
			return LineBuffer_X(0, m_arr_real);
		}
		constexpr size_t size() const {
			return m_Size_x * m_Size_y;
		}
		constexpr size_t capacity() const {
			return m_Capacity_x * m_Size_y;
		}
		constexpr size_t sizeX() const {
			return m_Size_x;
		}
		constexpr size_t capacityX() const {
			return m_Capacity_x;
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
			std::memset(m_arr_real.get(), NULL, sizeof(T) * capacity());
		}

		bool empty() {
			return m_arr_real ? false : true;
		}

		void swap(LineBuffer<T>& _swap) {
			size_t tempx = m_Size_x, tempy = m_Size_y, tempRx = m_Capacity_x;
			m_Size_x = _swap.m_Size_x;
			m_Size_y = _swap.m_Size_y;
			m_Capacity_x = _swap.m_Capacity_x;

			_swap.m_Size_x = tempx;
			_swap.m_Size_y = tempy;
			_swap.m_Capacity_x = tempRx;

			std::swap(m_arr_real, _swap.m_arr_real);
		}

		void add(LineBuffer<float>& _add) {
			if (CPUSupportChecker::AVX2()) {
				avx_add(_add);
			}
			else {
				sequential_add(_add);
			}
		}

		void mul(float _mul) {
			if (CPUSupportChecker::AVX2()) {
				avx_mul(_mul);
			}
			else {
				sequential_mul(_mul);
			}
		}

	private:
		std::unique_ptr<T[]> m_arr_real;
		size_t m_Size_x = 0, m_Size_y = 0, m_Capacity_x = 0;

		void create(const size_t _y, const size_t _x) {
			m_Size_x = _x == 0 ? 1 : _x;
			m_Size_y = _y == 0 ? 1 : _y;
			m_Capacity_x = calcAlienRange(m_Size_x);
			m_arr_real = std::make_unique<T[]>(m_Capacity_x * m_Size_y);
		}

		void avx_add(LineBuffer<float>& _add) {
			const size_t processFrames = std::min BOOST_PREVENT_MACRO_SUBSTITUTION(this->capacityX(), _add.capacityX());
			const size_t avxForAdd = sizeof(__m256) / sizeof(float);

			for (uint32_t chan = 0; chan < sizeY(); ++chan) {
				float* ptrThis = &at(chan).front();
				float* ptrAdd = &_add.at(chan).front();
				for (uint32_t fram = 0; fram < processFrames; fram += avxForAdd) {
					__m256 avxThis = _mm256_load_ps(ptrThis);
					__m256 avxAdd = _mm256_load_ps(ptrAdd);
					_mm256_store_ps(ptrThis, _mm256_add_ps(avxThis, avxAdd));
					ptrThis += avxForAdd;
					ptrAdd += avxForAdd;
				}
			}
		}

		void sequential_add(LineBuffer<float>& _add) {
			const size_t processFrames = std::min BOOST_PREVENT_MACRO_SUBSTITUTION(this->sizeX(), _add.sizeX());

			for (uint32_t chan = 0; chan < sizeY(); ++chan) {
				float* ptrThis = &at(chan).front();
				float* ptrAdd = &_add.at(chan).front();
				for (uint32_t fram = 0; fram < processFrames; ++fram) {
					*ptrThis += *ptrAdd;
					ptrThis++;
					ptrAdd++;
				}
			}
		}

		void avx_mul(float _mul) {
			const uint32_t avxForAdd = sizeof(__m256) / sizeof(float);
			const __m256 mul = _mm256_set_ps(_mul, _mul, _mul, _mul, _mul, _mul, _mul, _mul);

			for (uint32_t chan = 0; chan < sizeY(); ++chan) {
				float* ptrThis = &at(chan).front();
				for (uint32_t fram = 0; fram < capacityX(); fram += avxForAdd) {
					__m256 avxThis = _mm256_load_ps(ptrThis);
					_mm256_store_ps(ptrThis, _mm256_mul_ps(avxThis, mul));
					ptrThis += avxForAdd;
				}
			}
		}

		void sequential_mul(float _mul) {
			for (uint32_t chan = 0; chan < sizeY(); ++chan) {
				float* ptrThis = &at(chan).front();
				for (uint32_t fram = 0; fram < sizeX(); ++fram) {
					*ptrThis *= _mul;
					ptrThis++;
				}
			}
		}
	};
};
//要素番号探索式((y*m_Size_x)+x)

#endif