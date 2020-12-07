#include <nds.h>
#include <cstdio>

#include "solutions/sol.hpp"

int main() {
	touchPosition touch;
	PrintConsole console;
	
	videoSetMode(MODE_0_2D);
	videoSetModeSub(MODE_0_2D);

	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);

	consoleInit(&console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);

	consoleSelect(&console);
	for (int d = 0; d < 7; d++) {
		runSolution(d);
	}
	
	while(1) {
		touchRead(&touch);
		
		swiWaitForVBlank();
		scanKeys();
		
		int keys = keysDown();
		
		if(keys & KEY_START) break;
	}

	return 0;
}
