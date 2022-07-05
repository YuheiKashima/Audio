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
		bool GetPress(byte _key);
		bool GetTrigger(byte _key);
		bool GetRelease(byte _key);
	private:
		const byte m_Compare = 0x80;
		std::array<byte, 256> m_Primary, m_Secondry;
	};
}
#endif
