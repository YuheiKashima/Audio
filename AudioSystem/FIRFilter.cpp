#include "FIRFilter.h"

void HanningWindow() {
}

void FIR() {
}

void FIRDemo() {
	//�G�b�W���g��
	double edgeFreq = 1000.0;
	//�J�ڑш敝
	double delta = 1000.0;
	//�x���퐔
	uint32_t delayDevice = static_cast<uint32_t>(3.1 / delta + 0.5);

	//delayDevice+1����ɂȂ�悤����
	delayDevice % 2 == 1 ? delayDevice + 1 : delayDevice;

	//HanningWindow

	//�ȉ�FIR
}