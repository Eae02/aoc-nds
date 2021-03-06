#include "sol.hpp"
#include "../common.hpp"
#include "../read_input.hpp"
#include "../console.hpp"

#include <stdio.h>
#include <nds/timers.h>
#include <memory>

void AnswerBuffer::answerWithInts(int part1, int part2) {
	snprintf(ans1, sizeof(ans1), "%d", part1);
	snprintf(ans2, sizeof(ans2), "%d", part2);
}

#define XM_DAY(d) bool solveDay ## d(std::string_view input, AnswerBuffer& ans);
#include "../solved_days_xm.inl"
#undef XM_DAY

Solution solutions[25] = { };

void(*setSolutionProgress)(int progress);

void initSolutions() {
	setSolutionProgress = console::setProgress;
#define XM_DAY(d) solutions[d - 1] = &solveDay ## d;
#include "../solved_days_xm.inl"
#undef XM_DAY
}

static int prevSolutionProgress = 0;

void testRunSolution(int day) {
	if (solutions[day - 1] == nullptr) {
		fprintf(stderr, "no solution for day %d\n", day);
		return;
	}
	std::string_view input = readInput(day);
	if (input.empty()) {
		fprintf(stderr, "no input for day %d\n", day);
		return;
	}
	AnswerBuffer ans;
	
	prevSolutionProgress = -1;
	setSolutionProgress = [] (int progress) {
		if (prevSolutionProgress != progress) {
			prevSolutionProgress = progress;
			fprintf(stderr, "solution progress: %d%%\n", progress);
		}
	};
	cpuStartTiming(0);
	bool ok = solutions[day - 1](input, ans);
	u64 elapsedTicks = cpuEndTiming();
	setSolutionProgress = console::setProgress;
	
	if (!ok) {
		fprintf(stderr, "solving day %d failed\n time: %lums\n", day, ticksToMS(elapsedTicks));
		return;
	}
	fprintf(stderr, "day %d test run:\n part 1: %s\n part 2: %s\n time: %lums\n", day, ans.ans1, ans.ans2, ticksToMS(elapsedTicks));
}
