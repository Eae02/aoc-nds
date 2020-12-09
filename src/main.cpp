#include <nds.h>
#include <cstdio>
#include <fat.h>

#include "res/bkg.h"
#include "res/font.h"
#include "solutions/sol.hpp"

int main() {
	//lcdMainOnBottom();
	
	touchPosition touch;
	PrintConsole console;
	
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_0_2D);
	
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);
	
	int bg3 = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	
	consoleInit(&console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
	
	if (!fatInitDefault()) {
		printf("failed to initialize libfat\n");
	}
	
	ReadInputCallback readInput = &readEmbeddedInput;
	dmaCopy(bkgBitmap, bgGetGfxPtr(bg3), 256 * 256);
	dmaCopy(bkgPal, BG_PALETTE, 256 * 2);
	
	consoleSelect(&console);
	
	int prevPy = 0;
	int scrollY = 0;
	int prevKeyState = 0;
	while(1) {
		touchRead(&touch);
		
		swiWaitForVBlank();
		scanKeys();
		
		int keys = keysHeld();
		int newKeys = keys & ~prevKeyState;
		
		if (newKeys & KEY_X) {
			consoleClear();
			scrollY = 0;
			for (int d = 1; d <= MAX_DAY_IMPLEMETED; d++) {
				runSolution(d, readInput);
			}
		}
		
		if (keys & KEY_TOUCH) {
			if (prevKeyState & KEY_TOUCH) {
				scrollY -= (int)touch.py - prevPy;
			}
			prevPy = touch.py;
		}
		
		if (keys & KEY_UP) {
			scrollY--;
		}
		if (keys & KEY_DOWN) {
			scrollY++;
		}
		
		bgSetScroll(console.bgId, 0, scrollY);
		bgUpdate();
		
		prevKeyState = keys;
	}

	return 0;
}
