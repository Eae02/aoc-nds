#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <assert.h>
#include <stdio.h>

bool solveDay2(std::string_view input, AnswerBuffer& ans) {
	int part1Ans = 0;
	int part2Ans = 0;
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (isBlank(line)) continue;
		
		size_t dashPos = line.find('-');
		size_t spacePos = line.find(' ');
		size_t colPos = line.find(':');
		assert(dashPos != std::string_view::npos && spacePos != std::string_view::npos && colPos != std::string_view::npos);
		
		int rangeLo = parseInt(line.substr(0));
		int rangeHi = parseInt(line.substr(dashPos + 1));
		char c = line[colPos - 1];
		
		std::string_view str = line.substr(colPos + 2);
		int freq = std::count(str.begin(), str.end(), c);
		part1Ans += (int)(freq >= rangeLo && freq <= rangeHi);
		part2Ans += (int)((str[rangeLo - 1] == c) != (str[rangeHi - 1] == c));
	}
	
	ans.answerWithInts(part1Ans, part2Ans);
	return true;
}
