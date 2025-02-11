#ifndef _INSTANTINPUT_
#define _INSTANTINPUT_

#include <Windows.h>
#include <WinUser.h>
#include <array>

namespace myLib {
	class InstantInput {
	public:
		InstantInput();
		~InstantInput();
		bool UpdateState();
		void Ready();
		bool GetPress(BYTE _key);
		bool GetTrigger(BYTE _key);
		bool GetRelease(BYTE _key);
	private:
		const BYTE m_Compare = 0x80;
		std::array<BYTE, 256> m_Primary, m_Secondry;
	};
}
#endif
