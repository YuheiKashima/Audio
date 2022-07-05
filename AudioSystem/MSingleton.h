#ifndef _SINGLETON_
#define _SINGLETON_

namespace myLib {
	template<class T>
	class Singleton {
	public:
		Singleton(const T*) = delete;						//�R�s�[�R���X�g���N�^�� delete �w��
		Singleton& operator=(const Singleton&) = delete;	//�R�s�[������Z�q�� delete �w��
		Singleton(T&&) = delete;							//���[�u�R���X�g���N�^�� delete �w��
		Singleton& operator=(Singleton&&) = delete;		//���[�u������Z�q�� delete �w��

		static T& GetInstance() {
			static T inst;
			return inst;
		}

	protected:
		Singleton() = default;
		virtual ~Singleton() = default;
	};
};
#endif