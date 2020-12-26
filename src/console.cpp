#include "console.hpp"
#include "common.hpp"
#include "res/console.h"
#include <nds.h>
#include <stdio.h>

console::SolutionState console::solStates[25];

static int maxScroll;

constexpr int PIXELS_Y_HEADER = 3 * 8;
constexpr int PIXELS_Y_PER_DAY = 8 * 5;

static bool dayHasLongOutput[26] = { };

void irqInterruptHandler() {
	touchPosition touch;
	touchRead(&touch);
	scanKeys();
	console::updateInput(touch.px, touch.py);
	console::update();
}

void console::init() {
	dayHasLongOutput[14] = true;
	dayHasLongOutput[18] = true;
	dayHasLongOutput[21] = true;
	
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
		if (dayHasLongOutput[d + 1])
			maxScroll += 16;
	}
	maxScroll = std::max(maxScroll - SCREEN_HEIGHT, 0);
	
	irqSet(IRQ_VBLANK, irqInterruptHandler);
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
static volatile int dayToRun = -1;

void console::updateOutsideIRQ() {
	int dayToRunLocal = dayToRun;
	if (dayToRunLocal != -1) {
		if (solStates[dayToRunLocal].state == RunState::NotRun) {
			console::runSolution(dayToRunLocal);
		}
		dayToRunLocal = -1;
	}
}

void console::updateInput(int touchX, int touchY) {
	int keys = keysHeld();
	bool touching = (keys & KEY_TOUCH) != 0;
	
	dayToRun = -1;
	if (touching && !wasTouching) {
		int tapToRunLoY = PIXELS_Y_HEADER - scrollY;
		for (int d = 0; d < 25; d++) {
			if (solutions[d] == nullptr) continue;
			int tapToRunLoX = ((d < 10) ? 8 : 9) * 8;
			int tapToRunHiX = tapToRunLoX + 10 * 8;
			int tapToRunHiY = tapToRunLoY + 8 * 2;
			if (touchX >= tapToRunLoX && touchX <= tapToRunHiX && touchY >= tapToRunLoY && touchY <= tapToRunHiY) {
				dayToRun = d;
				break;
			}
			tapToRunLoY += PIXELS_Y_PER_DAY;
			if (dayHasLongOutput[d + 1])
				tapToRunLoY += 16;
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

static inline void clearLineData() {
	std::fill_n(allLineData[0], sizeof(allLineData) / 2, consoleMap[1000]);
}

static volatile int solutionProgress = -1;

static constexpr size_t BYTES_FIRST_LINE = sizeof(u16) * 32 * 2;

void console::runAllNoInteraction() {
	irqDisable(IRQ_VBLANK);
	REG_BG0VOFS = 0;
	clearLineData();
	dmaFillHalfWords(consoleMap[1000], (u16*)SCREEN_BASE_BLOCK(0), sizeof(allLineData));
	
	setSolutionProgress = [] (int) {};
	
	for (int d = 0; d < 25; d++) {
		if (solutions[d] == nullptr)
			continue;
		char msgBuffer[64];
		snprintf(msgBuffer, sizeof(msgBuffer), "running on day %d...", d + 1);
		setLineData(0, msgBuffer);
		DC_FlushRange(allLineData, BYTES_FIRST_LINE);
		dmaCopy(allLineData, (void*)SCREEN_BASE_BLOCK(0), BYTES_FIRST_LINE);
		
		runSolution(d);
	}
	
	setSolutionProgress = console::setProgress;
	
	irqEnable(IRQ_VBLANK);
	console::update();
}

void console::setProgress(int progress) {
	if (progress == solutionProgress)
		return;
	solutionProgress = progress;
}

void console::reset() {
	dayToRun = -1;
	std::fill_n(solStates, 25, SolutionState());
}

void console::runSolution(int day) {
	if (solutions[day] == nullptr)
		return;
	solutionProgress = -1;
	solStates[day].state = RunState::Running;
	solStates[day].failedMessage = nullptr;
	
	std::string_view input = readInput(day + 1);
	if (input.empty()) {
		solStates[day].failedMessage = "no input";
	} else {
		cpuStartTiming(0);
		bool ok = solutions[day](input, solStates[day].answers);
		uint32_t elapsedTicks = cpuEndTiming();
		solStates[day].elapsedTime = ticksToMS(elapsedTicks);
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
	
	clearLineData();
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
			if (solutionProgress != -1) {
				snprintf(lineBuffer, sizeof(lineBuffer), "Day \e%d\e: running (%d%%)%s", d + 1, solutionProgress, dotDotDot);
			} else {
				snprintf(lineBuffer, sizeof(lineBuffer), "Day \e%d\e: running%s", d + 1, dotDotDot);
			}
			break;
		case RunState::Waiting:
			snprintf(lineBuffer, sizeof(lineBuffer), "Day \e%d\e: waiting", d + 1);
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
		
		int lineLenBP = strlen(part1Ans) + strlen(part2Ans) + 9;
		
		if (d + 1 == 21 && lineLenBP > 32) {
			int charsPart2 = 32 - strlen(part1Ans) - 9;
			snprintf(lineBuffer, sizeof(lineBuffer), " P1=\e%s\e, P2=\e%.*s\e", part1Ans, charsPart2, part2Ans);
			setLineData(nextHalfLine + 2, lineBuffer);
			snprintf(lineBuffer, sizeof(lineBuffer), "\e%s\e", part2Ans + charsPart2);
			setLineData(nextHalfLine + 4, lineBuffer);
			nextHalfLine += 7;
		} else if (dayHasLongOutput[d + 1]) {
			snprintf(lineBuffer, sizeof(lineBuffer), " P1=\e%s\e", part1Ans);
			setLineData(nextHalfLine + 2, lineBuffer);
			snprintf(lineBuffer, sizeof(lineBuffer), " P2=\e%s\e", part2Ans);
			setLineData(nextHalfLine + 4, lineBuffer);
			nextHalfLine += 7;
		} else {
			snprintf(lineBuffer, sizeof(lineBuffer), " P1=\e%s\e, P2=\e%s\e", part1Ans, part2Ans);
			setLineData(nextHalfLine + 2, lineBuffer);
			nextHalfLine += 5;
		}
	}
	
	DC_FlushRange(allLineData, sizeof(allLineData));
	dmaCopy(allLineData, (void*)SCREEN_BASE_BLOCK(0), sizeof(allLineData));
}
