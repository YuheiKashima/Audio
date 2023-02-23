#include "FIRFilter.h"

void HanningWindow() {
}

void FIR() {
}

void FIRDemo() {
	//ƒGƒbƒWü”g”
	double edgeFreq = 1000.0;
	//‘JˆÚ‘Ñˆæ•
	double delta = 1000.0;
	//’x‰„Ší”
	uint32_t delayDevice = static_cast<uint32_t>(3.1 / delta + 0.5);

	//delayDevice+1‚ªŠï”‚É‚È‚é‚æ‚¤’²®
	delayDevice % 2 == 1 ? delayDevice + 1 : delayDevice;

	//HanningWindow

	//ˆÈ‰ºFIR
}