#include <nds.h>
#include <cstdio>

#include "res/bkg.h"
#include "console.hpp"
#include "solutions/sol.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main() {
	lcdMainOnBottom();
	
	videoSetModeSub(MODE_5_2D);
	vramSetBankC(VRAM_C_SUB_BG_0x06200000);
	
	consoleDebugInit(DebugDevice_NOCASH);
	initInputReading();
	
	initSolutions();
	console::init();
	
	int topScreenBg = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	
	dmaCopy(bkgBitmap, bgGetGfxPtr(topScreenBg), 256 * 256);
	dmaCopy(bkgPal, BG_PALETTE_SUB, 256 * 2);
	
	while(1) {
		swiWaitForVBlank();
		
		uint32_t keys = keysDown();
		
		if ((keys & KEY_A) != 0) {
			console::runAllNoInteraction();
		}
		
		console::updateOutsideIRQ();
	}

	return 0;
}
