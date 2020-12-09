#include "console.hpp"
#include "res/console.h"
#include <nds.h>
#include <stdio.h>

console::SolutionState console::solStates[25];

static int maxScroll;

constexpr int PIXELS_Y_HEADER = 3 * 8;
constexpr int PIXELS_Y_PER_DAY = 8 * 5;

void console::init() {
	videoSetMode(MODE_5_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankA(VRAM_A_MAIN_BG);
	
	BGCTRL[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32;
	
	dmaCopy(consoleTiles, (void*)CHAR_BASE_BLOCK(1), consoleTilesLen);
	dmaFillHalfWords(0, (void*)SCREEN_BASE_BLOCK(0), consoleMapLen);
	dmaCopy(consolePal,BG_PALETTE,sizeof(consolePal));
	console::update();
	
	maxScroll = PIXELS_Y_HEADER;
	for (int d = 0; d < 25; d++) {
		if (solutions[d] != nullptr)
			maxScroll += PIXELS_Y_PER_DAY;
	}
	maxScroll = std::max(maxScroll - SCREEN_HEIGHT, 0);
}

static u16 allLineData[32][32];

void setLineData(int line, const char* data) {
	bool bold = false;
	for (int i = 0; i < 32; i++) {
		if (*data == '\e') {
			bold = !bold;
			data++;
		}
		
		int charIdx = 95;
		if (*data >= '!' && *data <= '~')
			charIdx = *data - '!';
		
		int charCol = charIdx % 32;
		int charRow = charIdx / 32;
		if (bold) charRow += 3;
		
		
		if (line >= 0 && line < 32)
			allLineData[line + 0][i] = consoleMap[(charRow * 64) + charCol];
		if (line >= -1 && line < 31)
			allLineData[line + 1][i] = consoleMap[(charRow * 64) + 32 + charCol];
		
		if (*data != '\0') {
			data++;
		}
	}
}

static volatile int prevTouchY = 0;
static volatile bool wasTouching = false;
static volatile int scrollY = 0;

void console::updateInput(int touchX, int touchY) {
	int keys = keysHeld();
	bool touching = (keys & KEY_TOUCH) != 0;
	
	if (touching && !wasTouching) {
		for (int d = 0; d < 25; d++) {
			if (solStates[d].state != RunState::NotRun)
				continue;
			int tapToRunLoX = ((d < 10) ? 8 : 9) * 8;
			int tapToRunHiX = tapToRunLoX + 10 * 8;
			int tapToRunLoY = PIXELS_Y_HEADER + d * PIXELS_Y_PER_DAY - scrollY;
			int tapToRunHiY = tapToRunLoY + 8 * 2;
			if (touchX >= tapToRunLoX && touchX <= tapToRunHiX && touchY >= tapToRunLoY && touchY <= tapToRunHiY) {
				console::runSolution(d);
				break;
			}
		}
	}
	
	constexpr int SCROLL_PER_FRAME = 4;
	if (keys & KEY_UP) {
		scrollY -= SCROLL_PER_FRAME;
	}
	if (keys & KEY_DOWN) {
		scrollY += SCROLL_PER_FRAME;
	}
	if (touching && wasTouching) {
		scrollY += prevTouchY - touchY;
	}
	if (scrollY < 0) scrollY = 0;
	if (scrollY > maxScroll) scrollY = maxScroll;
	
	wasTouching = touching;
	prevTouchY = touchY;
}

static volatile int consoleFrameIdx = 0;
static volatile int numDots = 0;
static constexpr int FRAMES_PER_DOT = 30;

void console::runSolution(int day) {
	if (solutions[day] == nullptr)
		return;
	solStates[day].state = RunState::Running;
	solStates[day].failedMessage = nullptr;
	
	std::string_view input = readInput(day + 1);
	if (input.empty()) {
		solStates[day].failedMessage = "no input";
	} else {
		cpuStartTiming(0);
		bool ok = solutions[day](input, solStates[day].answers);
		uint32_t elapsedTicks = cpuEndTiming();
		solStates[day].elapsedTime = elapsedTicks * 1000 / BUS_CLOCK;
		if (!ok) {
			solStates[day].failedMessage = "failed";
		}
	}
	
	solStates[day].state = RunState::Done;
}

void console::update() {
	consoleFrameIdx++;
	if (consoleFrameIdx == FRAMES_PER_DOT) {
		consoleFrameIdx = 0;
		numDots = (numDots + 1) % 3;
	}
	
	char dotDotDot[4] = "...";
	dotDotDot[numDots + 1] = '\0';
	
	std::fill_n(allLineData[0], sizeof(allLineData) / 2, consoleMap[1000]);
	char lineBuffer[64];
	
	constexpr int LINE_PIXELS_H = 8;
	REG_BG0VOFS = (scrollY % LINE_PIXELS_H);
	
	int nextHalfLine = -(scrollY / LINE_PIXELS_H);
	setLineData(nextHalfLine, "Press \eA\e to run all solutions");
	nextHalfLine += 3;
	
	for (int d = 0; d < 25; d++) {
		if (solutions[d] == nullptr)
			continue;
		
		const char* part1Ans = "n/a";
		const char* part2Ans = "n/a";
		
		switch (solStates[d].state) {
		case RunState::NotRun:
			snprintf(lineBuffer, sizeof(lineBuffer), "Day \e%d\e: [\etap to run\e]", d + 1);
			break;
		case RunState::Running:
			snprintf(lineBuffer, sizeof(lineBuffer), "Day \e%d\e: running%s", d + 1, dotDotDot);
			break;
		case RunState::Waiting:
			snprintf(lineBuffer, sizeof(lineBuffer), "Day \e%d\e: waiting%s", d + 1, dotDotDot);
			break;
		case RunState::Done:
			if (solStates[d].failedMessage) {
				snprintf(lineBuffer, sizeof(lineBuffer), "Day \e%d\e: %s", d + 1, solStates[d].failedMessage);
			} else {
				snprintf(lineBuffer, sizeof(lineBuffer), "Day \e%d\e: done in \e%lu\ems", d + 1, solStates[d].elapsedTime);
				part1Ans = solStates[d].answers.ans1;
				part2Ans = solStates[d].answers.ans2;
			}
			break;
		}
		setLineData(nextHalfLine, lineBuffer);
		
		snprintf(lineBuffer, sizeof(lineBuffer), " P1=\e%s\e, P2=\e%s\e", part1Ans, part2Ans);
		setLineData(nextHalfLine + 2, lineBuffer);
		
		nextHalfLine += 5;
	}
	
	DC_FlushRange(allLineData, sizeof(allLineData));
	dmaCopy(allLineData, (void*)SCREEN_BASE_BLOCK(0), sizeof(allLineData));
}
