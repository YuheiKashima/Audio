#include "FIRFilter.h"

void HanningWindow() {
}

void FIR() {
}

void FIRDemo() {
	//GbWüg
	double edgeFreq = 1000.0;
	//JÚÑæ
	double delta = 1000.0;
	//xí
	uint32_t delayDevice = static_cast<uint32_t>(3.1 / delta + 0.5);

	//delayDevice+1ªïÉÈéæ¤²®
	delayDevice % 2 == 1 ? delayDevice + 1 : delayDevice;

	//HanningWindow

	//ÈºFIR
}