#include "EffectBase.h"

namespace AS {
	class FIRFilter {
	public:
		FIRFilter();
		~FIRFilter();

		float Process(float _src);
		void Flush();
	private:
	};
}