#ifndef _MCIRCULARBUFFER_
#define _MCIRCULARBUFFER_

#include <memory>

//�Q�l
//http://precure-3dprinter.hatenablog.jp/entry/2018/02/12/%E4%BD%BF%E3%81%86%E3%81%AE%E3%81%8B%E5%88%86%E3%81%8B%E3%82%89%E3%81%AA%E3%81%84%E3%83%AA%E3%83%B3%E3%82%B0%E3%83%90%E3%83%83%E3%83%95%E3%82%A1%E3%81%AE%E3%83%A9%E3%82%A4%E3%83%96%E3%83%A9
//https://mickey24.hatenablog.com/entry/20081021/1224590753
namespace myLib {
	template<typename T>
	class CircularBuffer {
	public:
		CircularBuffer<T>() {}
		explicit CircularBuffer<T>(size_t _n) {
			create(_n);
		}
		CircularBuffer<T>(const CircularBuffer<T>& _right) {
			copy(_right);
		}
		~CircularBuffer() {
			m_Buffer.reset();
		}
		//�N�_����������̗v�f�����
		T operator [](int32_t _idx)const {
			return m_Buffer[index(_idx)];
		}
		//�N�_����������̗v�f�����
		T& operator [](int32_t _idx) {
			return m_Buffer[index(_idx)];
		}
		operator const T* ()const {
			return m_Buffer;
		}
		//�o�b�t�@���R�s�[
		CircularBuffer<T>& operator =(const CircularBuffer<T>& _right) {
			copy(_right); return *this;
		}
		//�擪�C���f�b�N�X����
		CircularBuffer<T>& operator ++() {
			set(1);
			return *this;
		}
		//�擪�C���f�b�N�X����
		CircularBuffer<T> operator ++(int) {
			auto t = *this;
			set(1);
			return t;
		}
		//�擪�C���f�b�N�X�߂�
		CircularBuffer<T>& operator --() {
			set(-1);
			return *this;
		}

		//�擪�C���f�b�N�X�߂�
		CircularBuffer<T> operator --(int) {
			auto t = *this;
			set(-1);
			return t;
		}

		//�o�b�t�@���T�C�Y
		void resize(size_t _n) {
			m_Buffer.reset();
			create(_n);
		}
		//�o�b�t�@�R�s�[
		void copy(const CircularBuffer<T>& _right) {
			if (m_Buffer) {
				m_Buffer.reset();
			}
			if (!_right.empty()) {
				create(_right.m_Realsize);
				m_Beg = _right.m_Beg;
				m_size = _right.m_size;
				m_Realsize = _right.m_Realsize;
				m_reverse = _right.m_reverse;
				memcpy(m_Buffer, _right.m_Buffer, sizeof(T) * m_Realsize);
			}
		}
		//�o�b�t�@�[���N���A
		void clear() {
			m_Beg = 0;
			m_reverse = false;
			memset(m_Buffer.get(), NULL, sizeof(T) * m_Realsize);
		}
		//�C���f�b�N�X�Z�b�g(���Έʒu)
		void set(const int32_t _idx) {
			m_Beg = index(_idx);
		}
		//��`�F�b�N
		bool empty() {
			return m_Buffer == nullptr ? true : false;
		}
		//�o�b�t�@����������]
		void reverse() {
			m_reverse != m_reverse;
		}
		//�T�C�Y�擾
		size_t size()const {
			return m_size;
		}
	private:
		std::unique_ptr<T[]> m_Buffer = nullptr;
		size_t m_size = 0;
		size_t m_Realsize = 0;
		int32_t m_Beg = 0;
		bool m_reverse = false;

		void create(size_t _n) {
			m_Realsize = calcAlienRange(_n);
			m_size = _n;
			m_Buffer = std::make_unique<T[]>(m_Realsize);

			clear();
		}
		//�C���f�b�N�X�v�Z
		int32_t index(int32_t _idx) {
			int32_t idx = m_reverse ? -(_idx + 1) : _idx;
			return m_Beg + idx >= 0 ?
				(m_Beg + idx) % m_size :
				(m_Beg + (m_size - std::abs(idx) % m_size)) % m_size;
		}
	};
};
#endif