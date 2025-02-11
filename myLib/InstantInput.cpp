#include "InstantInput.h"

using namespace myLib;
using namespace std;

InstantInput::InstantInput() :m_Primary{}, m_Secondry{} {
}

InstantInput::~InstantInput() {
}

bool InstantInput::UpdateState() {
	bool res = GetKeyboardState(static_cast<PBYTE>(m_Primary.data()));
	return res;
}

void InstantInput::Ready() {
	std::copy(m_Primary.begin(), m_Primary.end(), m_Secondry.begin());
}

bool InstantInput::GetPress(BYTE _key) {
	return m_Primary[_key] & m_Compare ? true : false;
}

bool InstantInput::GetTrigger(BYTE _key) {
	return (m_Primary[_key] & m_Compare) && !(m_Secondry[_key] & m_Compare) ? true : false;
}

bool InstantInput::GetRelease(BYTE _key) {
	return !(m_Primary[_key] & m_Compare) && (m_Secondry[_key] & m_Compare) ? true : false;
}