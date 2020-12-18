#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <stdio.h>

uint32_t countTrees(std::string_view input, uint32_t dx, uint32_t dy) {
	uint32_t lineLen = input.find('\n');
	uint32_t numLines = std::count(input.begin(), input.end(), '\n');
	
	uint32_t numTrees = 0;
	for (uint32_t x = 0, y = 0; y < numLines; y += dy, x += dx) {
		uint32_t xm = x % lineLen;
		uint32_t idx = xm + y * (lineLen + 1);
		numTrees += (uint32_t)(input[idx] == '#');
	}
	return numTrees;
}

bool solveDay3(std::string_view input, AnswerBuffer& ans) {
	uint32_t lineLen = input.find('\n');
	uint32_t numLines = std::count(input.begin(), input.end(), '\n');
	uint32_t r1d1 = 0, r3d1 = 0, r5d1 = 0, r7d1 = 0, r1d2 = 0;
	
	const char* line = input.data();
	for (uint32_t y = 0; y < numLines; y++, line += lineLen + 1) {
		auto isTree = [&] (uint32_t x) { return line[x % lineLen] == '#'; };
		r1d1 += isTree(y);
		r3d1 += isTree(y * 3);
		r5d1 += isTree(y * 5);
		r7d1 += isTree(y * 7);
		if ((y % 2) == 0)
			r1d2 += isTree(y / 2);
	}
	
	uint64_t ans2 = (uint64_t)r1d1 * (uint64_t)r3d1 * (uint64_t)r5d1 * (uint64_t)r7d1 * (uint64_t)r1d2;
	
	snprintf(ans.ans1, sizeof(ans.ans1), "%lu", r3d1);
	snprintf(ans.ans2, sizeof(ans.ans2), "%llu", ans2);
	return true;
}
