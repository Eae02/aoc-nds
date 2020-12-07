#include "sol.hpp"
#include "../inputs/1.txt.h"
#include "../inputs/2.txt.h"
#include "../inputs/3.txt.h"
#include "../inputs/4.txt.h"
#include "../inputs/5.txt.h"
#include "../inputs/6.txt.h"
#include "../inputs/7.txt.h"

#include <nds/timers.h>
#include <stdio.h>

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

Solution solutions[25] = {
	&solveDay1,
	&solveDay2,
	&solveDay3,
	&solveDay4,
	&solveDay5,
	&solveDay6,
	&solveDay7
};

std::string_view inputs[25] = {
	std::string_view((const char*)inputs_1_txt, inputs_1_txt_len),
	std::string_view((const char*)inputs_2_txt, inputs_2_txt_len),
	std::string_view((const char*)inputs_3_txt, inputs_3_txt_len),
	std::string_view((const char*)inputs_4_txt, inputs_4_txt_len),
	std::string_view((const char*)inputs_5_txt, inputs_5_txt_len),
	std::string_view((const char*)inputs_6_txt, inputs_6_txt_len),
	std::string_view((const char*)inputs_7_txt, inputs_7_txt_len)
};

void runSolution(int day) {
	if (solutions[day] == nullptr) {
		printf("day %d is not implemented!\n", day + 1);
		return;
	}
	
	printf("solving day %d..", day + 1);
	AnswerBuffer ans;
	cpuStartTiming(0);
	bool ok = solutions[day](inputs[day], ans);
	uint32_t elapsedTicks = cpuEndTiming();
	if (ok) {
		uint32_t elapsedMS = elapsedTicks * 1000 / BUS_CLOCK;
		printf(" done in %ldms\n part 1: %s\n part 2: %s\n", elapsedMS, ans.ans1, ans.ans2);
	} else {
		printf(" failed!\n");
	}
}
