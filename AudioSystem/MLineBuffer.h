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
		LineBuffer<T>();
		//x*yの1次元配列を生成してすべての配列をvalueにする
		LineBuffer<T>(const size_t _y, const size_t _x, const T _value);
		//x*yの1次元配列を生成するだけ
		constexpr LineBuffer<T>(const size_t _y, const size_t _x);
		//2次元配列に見立てた1次元配列のx軸の先頭のLineBuffer_Xを入手(1つ目の[])
		constexpr const LineBuffer_X<T> operator[](const size_t _y) const;
		constexpr const LineBuffer<T>& operator =(const LineBuffer<T>& src);
		constexpr T* operator&() const;

		constexpr const LineBuffer_X<T> at(const size_t _y) const;
		constexpr const LineBuffer_X<T> front() const;
		constexpr size_t size() const;
		constexpr size_t realSize() const;
		constexpr size_t sizeX() const;
		constexpr size_t sizeRealX() const;
		constexpr size_t sizeY() const;
		constexpr T* const get() const;
		constexpr T* const begin() const;
		constexpr T* const end() const;

		void resize(const size_t _y, const size_t _x);
		void zeroclear();
		std::unique_ptr<T[]> data() const;
		bool empty();
		void swap(LineBuffer<T>& _swap);

		void avx_add(LineBuffer<float>& _add);
		void avx_mul(float _mul);

	private:
		std::unique_ptr<T[]> m_arr_real;
		size_t m_Size_x = 0, m_Size_y = 0, m_RealSize_x = 0;

		void create(const size_t _y, const size_t _x);
	};

	template<typename T>
	LineBuffer<T>::LineBuffer() {
	}

	template<typename T>
	LineBuffer<T>::LineBuffer(const size_t _y, const size_t _x, const T _value) {
		create(_y, _x);
		std::memset(m_arr_real.get(), _value, sizeof(T) * m_Size_x * m_Size_y);
	}

	template<typename T>
	constexpr LineBuffer<T>::LineBuffer(const size_t _y, const size_t _x) {
		create(_y, _x);
	}

	template<typename T>
	constexpr const LineBuffer_X<T> LineBuffer<T>::operator[](const size_t _y) const {
		LineBuffer_X<T> arr_x(_y * m_Size_x, m_arr_real);
		return arr_x;
	}

	template<typename T>
	constexpr const LineBuffer<T>& LineBuffer<T>::operator =(const LineBuffer<T>& _src) {
		if (realSize() != _src.realSize()) {
			m_arr_real.reset();
			create(_src.m_Size_y, _src.m_Size_x);
		}
		memcpy_s(m_arr_real.get(), sizeof(T) * m_Size_x * m_Size_y, _src.m_arr_real.get(), sizeof(T) * _src.m_Size_x * _src.m_Size_y);
		return *this;
	}

	template<typename T>
	constexpr T* LineBuffer<T>::operator&() const {
		return m_arr_real.get();
	}

	template<typename T>
	void LineBuffer<T>::create(const size_t _y, const size_t _x) {
		m_Size_x = _x == 0 ? 1 : _x;
		m_Size_y = _y == 0 ? 1 : _y;
		m_RealSize_x = calcAlienRange(m_Size_x);
		m_arr_real = std::make_unique<T[]>(m_RealSize_x * m_Size_y);
	}

	template<typename T>
	constexpr const LineBuffer_X<T> LineBuffer<T>::at(const size_t _y) const {
		return LineBuffer_X(_y * m_Size_x, m_arr_real);
	}

	template<typename T>
	constexpr const LineBuffer_X<T> LineBuffer<T>::front() const {
		return LineBuffer_X(0, m_arr_real);
	}

	template<typename T>
	constexpr size_t LineBuffer<T>::size() const {
		return m_Size_x * m_Size_y;
	}

	template<typename T>
	constexpr size_t LineBuffer<T>::realSize() const {
		return m_RealSize_x * m_Size_y;
	}

	template<typename T>
	constexpr size_t LineBuffer<T>::sizeX() const {
		return m_Size_x;
	}

	template<typename T>
	constexpr size_t LineBuffer<T>::sizeRealX() const {
		return m_RealSize_x;
	}

	template<typename T>
	constexpr size_t LineBuffer<T>::sizeY() const {
		return m_Size_y;
	}

	template<typename T>
	constexpr T* const LineBuffer<T>::get() const {
		return m_arr_real.get();
	}

	template<typename T>
	constexpr T* const LineBuffer<T>::begin() const {
		return m_arr_real.get();
	}

	template<typename T>
	constexpr T* const LineBuffer<T>::end() const {
		return m_arr_real.get() + m_Size_x * m_Size_y;
	}

	template<typename T>
	void LineBuffer<T>::resize(const size_t _y, const size_t _x) {
		m_arr_real.reset();
		create(_y, _x);
		zeroclear();
	}

	template<typename T>
	void LineBuffer<T>::zeroclear() {
		std::memset(m_arr_real.get(), NULL, sizeof(T) * realSize());
	}

	template<typename T>
	bool LineBuffer<T>::empty() {
		return m_arr_real ? false : true;
	}

	template<typename T>
	void LineBuffer<T>::swap(LineBuffer<T>& _swap) {
		size_t tempx = m_Size_x, tempy = m_Size_y, tempRx = m_RealSize_x;
		m_Size_x = _swap.m_Size_x;
		m_Size_y = _swap.m_Size_y;
		m_RealSize_x = _swap.m_RealSize_x;

		_swap.m_Size_x = tempx;
		_swap.m_Size_y = tempy;
		_swap.m_RealSize_x = tempRx;

		std::swap(m_arr_real, _swap.m_arr_real);
	}

	template<typename T>
	void LineBuffer<T>::avx_add(LineBuffer<float>& _add) {
		const auto processFrames = std::min(this->size(), _add.size());
		const auto avxForAdd = sizeof(__m256) / sizeof(float);

		auto ptrThis = this->get(), ptrAdd = _add.get();
		for (uint32_t i = 0; i < processFrames; i += avxForAdd) {
			__m256 avxThis = _mm256_load_ps(ptrThis), avxAdd = _mm256_load_ps(ptrAdd);
			_mm256_store_ps(ptrThis, _mm256_add_ps(avxThis, avxAdd));
			ptrThis += avxForAdd;
			ptrAdd += avxForAdd;
		}
	}

	template<typename T>
	void LineBuffer<T>::avx_mul(float _mul) {
		const uint32_t avxForAdd = sizeof(__m256) / sizeof(float);
		const __m256 mul = _mm256_set_ps(_mul, _mul, _mul, _mul, _mul, _mul, _mul, _mul);

		auto ptrThis = this->get();
		for (uint32_t i = 0; i < this->size(); i += avxForAdd) {
			__m256 avxThis = _mm256_load_ps(ptrThis);
			_mm256_store_ps(ptrThis, _mm256_mul_ps(avxThis, mul));

			ptrThis += avxForAdd;
		}
	}
};
//要素番号探索式((y*m_Size_x)+x)

#endif