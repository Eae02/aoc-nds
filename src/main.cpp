#include <nds.h>
#include <cstdio>

#include "res/bkg.h"
#include "console.hpp"
#include "solutions/sol.hpp"

int main() {
	lcdMainOnBottom();
	
	touchPosition touch;
	
	videoSetModeSub(MODE_5_2D);
	vramSetBankC(VRAM_C_SUB_BG_0x06200000);
	
	consoleDebugInit(DebugDevice_NOCASH);
	initInputReading();
	
	console::init();
	irqSet(IRQ_VBLANK, [] { console::update(); });
	
	int topScreenBg = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	
	dmaCopy(bkgBitmap, bgGetGfxPtr(topScreenBg), 256 * 256);
	dmaCopy(bkgPal, BG_PALETTE_SUB, 256 * 2);
	
	int runningOnDay = -1;
	
	int prevKeyState = 0;
	while(1) {
		swiWaitForVBlank();
		touchRead(&touch);
		scanKeys();
		
		int keys = keysHeld();
		int newKeys = keys & ~prevKeyState;
		
		console::updateInput(touch.px, touch.py);
		
		if (runningOnDay == -1 && (newKeys & KEY_A) != 0) {
			runningOnDay = 0;
			for (int d = 0; d < 25; d++) {
				console::solStates[d].state = console::RunState::Waiting;
			}
		}
		
		if (runningOnDay != -1) {
			console::runSolution(runningOnDay);
			runningOnDay++;
			if (runningOnDay == 25) {
				runningOnDay = -1;
			}
		}
		
		prevKeyState = keys;
	}

	return 0;
}
