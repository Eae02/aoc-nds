#pragma once

#include <string_view>

#include "../read_input.hpp"

struct AnswerBuffer {
	char ans1[64];
	char ans2[64];
	
	void answerWithInts(int part1, int part2);
};

using Solution = bool(*)(std::string_view input, AnswerBuffer& ans);

extern Solution solutions[25];

void initSolutions();

void testRunSolution(int day);
