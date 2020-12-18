#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <stdio.h>

bool solveDay5(std::string_view input, AnswerBuffer& ans) {
	int part1Ans = 0;
	std::bitset<1024> seen;
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (isBlank(line)) continue;
		
		int id = 0;
		for (int i = 0; i < 10; i++) {
			if (line[i] == 'R' || line[i] == 'B') {
				id |= 1 << (9 - i);
			}
		}
		
		part1Ans = std::max(part1Ans, id);
		seen.set(id);
	}
	
	int part2Ans = 0;
	for (int i = 1; i < (int)seen.size() - 1; i++) {
		if (seen[i - 1] && !seen[i] && seen[i + 1])
			part2Ans = i;
	}
	
	ans.answerWithInts(part1Ans, part2Ans);
	return true;
}
