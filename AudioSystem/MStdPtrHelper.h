#ifndef _MSTDPTRHELPER_
#define _MSTDPTRHELPER_

#include "Utilities.h"

/*プライベートなコンストラクタ、デストラクタのクラスをshared_ptrへmake_sharedはできない
(friendされたクラスからもfriendClass->shared_ptr->生成クラスの順でアクセスが発生するため不可)
解決
コンストラクタが見えているところに補助オブジェクトを定義しそれをmakesharedした後補助オブジェクト内の
使用オブジェクトへのshared_ptrを作成
→目的クラス(T)を継承した補助オブジェクト(entry)を目的クラスにポリモーフさせ渡す。
デメリット
アロケートメモリがStdPtrHelper::entry+<T>の量になる
*/
namespace myLib {
	template<typename T>
	class StdPtrHelper {
		friend T;

		struct entity : public T {
			template<typename... Args>
			entity(Args&&... args) : T(std::forward<Args>(args)...) { }
		};

		template<typename... Args>
		static std::shared_ptr<T> make_shared(Args&&... args) {
			return std::make_shared<entity>(std::forward<Args>(args)...);
		}

		template<typename Alloc, typename... Args>
		static std::shared_ptr<T> allocate_shared(const Alloc& alloc, Args&&... args) {
			return std::allocate_shared<entity>(alloc, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static std::unique_ptr<T> make_unique(Args&&... args) {
			return std::make_unique<entity>(std::forward<Args>(args)...);
		}
	};
};
#endif