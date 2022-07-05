#include "InstantInput.h"

myLib::InstantInput::InstantInput() :m_Primary{}, m_Secondry{} {
}

myLib::InstantInput::~InstantInput() {
}

bool myLib::InstantInput::UpdateState() {
	bool res = GetKeyboardState(m_Primary.data());
	return res;
}

void myLib::InstantInput::Ready() {
	std::copy(m_Primary.begin(), m_Primary.end(), m_Secondry.begin());
}

bool myLib::InstantInput::GetPress(byte _key) {
	return m_Primary[_key] & m_Compare ? true : false;
}

bool myLib::InstantInput::GetTrigger(byte _key) {
	return (m_Primary[_key] & m_Compare) && !(m_Secondry[_key] & m_Compare) ? true : false;
}

bool myLib::InstantInput::GetRelease(byte _key) {
	return !(m_Primary[_key] & m_Compare) && (m_Secondry[_key] & m_Compare) ? true : false;
}