#include "sol.hpp"
#include "../common.hpp"
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
bool solveDay10(std::string_view input, AnswerBuffer& ans);
bool solveDay11(std::string_view input, AnswerBuffer& ans);

Solution solutions[25] = {
	&solveDay1,
	&solveDay2,
	&solveDay3,
	&solveDay4,
	&solveDay5,
	&solveDay6,
	&solveDay7,
	&solveDay8,
	&solveDay9,
	&solveDay10,
	&solveDay11
};

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
	cpuStartTiming(0);
	bool ok = solutions[day - 1](input, ans);
	u64 elapsedTicks = cpuEndTiming();
	
	if (!ok) {
		fprintf(stderr, "solving day %d failed\n time: %lums\n", day, ticksToMS(elapsedTicks));
		return;
	}
	fprintf(stderr, "day %d test run:\n part 1: %s\n part 2: %s\n time: %lums\n", day, ans.ans1, ans.ans2, ticksToMS(elapsedTicks));
}
