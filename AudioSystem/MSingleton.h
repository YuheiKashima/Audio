#ifndef _SINGLETON_
#define _SINGLETON_

namespace myLib {
	template<class T>
	class Singleton {
	public:
		Singleton(const T*) = delete;						//コピーコンストラクタを delete 指定
		Singleton& operator=(const Singleton&) = delete;	//コピー代入演算子も delete 指定
		Singleton(T&&) = delete;							//ムーブコンストラクタを delete 指定
		Singleton& operator=(Singleton&&) = delete;		//ムーブ代入演算子も delete 指定

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