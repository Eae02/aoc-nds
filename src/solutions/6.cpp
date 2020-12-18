#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <stdio.h>

bool solveDay6(std::string_view input, AnswerBuffer& ans) {
	int part1Ans = 0;
	int part2Ans = 0;
	
	std::bitset<26> anyAnswered;
	std::bitset<26> allAnswered;
	allAnswered.set();
	
	while (true) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (isBlank(line)) {
			part1Ans += anyAnswered.count();
			part2Ans += allAnswered.count();
			anyAnswered.reset();
			allAnswered.set();
			if (input.empty()) break;
		} else {
			std::bitset<26> answeredHere;
			for (char c : line) {
				answeredHere.set(c - 'a');
			}
			anyAnswered |= answeredHere;
			allAnswered &= answeredHere;
		}
	}
	
	ans.answerWithInts(part1Ans, part2Ans);
	return true;
}
