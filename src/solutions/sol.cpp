#include "sol.hpp"
#include "../read_input.hpp"

#include <stdio.h>
#include <nds/timers.h>
#include <memory>

void AnswerBuffer::answerWithInts(int part1, int part2) {
	snprintf(ans1, sizeof(ans1), "%d", part1);
	snprintf(ans2, sizeof(ans2), "%d", part2);
}

bool solveDay1(std::string_view input, AnswerBuffer& ans);
bool solveDay2(std::string_view input, AnswerBuffer& ans);
bool solveDay3(std::string_view input, AnswerBuffer& ans);
bool solveDay4(std::string_view input, AnswerBuffer& ans);
bool solveDay5(std::string_view input, AnswerBuffer& ans);
bool solveDay6(std::string_view input, AnswerBuffer& ans);
bool solveDay7(std::string_view input, AnswerBuffer& ans);
bool solveDay8(std::string_view input, AnswerBuffer& ans);
bool solveDay9(std::string_view input, AnswerBuffer& ans);

Solution solutions[25] = {
	&solveDay1,
	&solveDay2,
	&solveDay3,
	&solveDay4,
	&solveDay5,
	&solveDay6,
	&solveDay7,
	&solveDay8,
	&solveDay9
};

void runSolution(int day, ReadInputCallback readInput) {
	if (solutions[day - 1] == nullptr) {
		printf("day %d is not implemented!\n", day);
		return;
	}
	
	std::string_view input = readInput(day);
	if (input.empty()) {
		printf("input for day %d is not available\n", day);
		return;
	}
	
	printf("solving day %d..", day);
	AnswerBuffer ans;
	cpuStartTiming(0);
	bool ok = solutions[day - 1](input, ans);
	uint32_t elapsedTicks = cpuEndTiming();
	if (ok) {
		uint32_t elapsedMS = elapsedTicks * 1000 / BUS_CLOCK;
		printf(" done in %ldms\n part 1: %s\n part 2: %s\n", elapsedMS, ans.ans1, ans.ans2);
	} else {
		printf(" failed!\n");
	}
}
