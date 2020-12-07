#include "sol.hpp"

#include <cstdio>
#include <cstring>
#include <charconv>
#include <bitset>
#include <algorithm>

bool solveDay1(std::string_view input, AnswerBuffer& ans) {
	std::bitset<2020> seen;
	
	uint32_t numLines = std::count(input.begin(), input.end(), '\n');
	std::vector<int> valuesLessThan1010;
	valuesLessThan1010.reserve(numLines);
	int num1010 = 0;
	
	const char* remInput = input.data();
	while (remInput < &input.back()) {
		if (isspace(*remInput)) {
			remInput++;
			continue;
		}
		
		int value;
		auto parseRes = std::from_chars(remInput, &input.back(), value);
		if (parseRes.ec != std::errc()) {
			printf("parse error!");
			return false;
		}
		remInput = parseRes.ptr;
		
		if (value <= 2020) {
			seen.set(value);
			if (value < 1010) {
				valuesLessThan1010.push_back(value);
			} else if (value == 1010) {
				num1010++;
			}
		}
	}
	
	int part1Ans = -1, part2Ans = -1;
	if (num1010 >= 2) {
		part1Ans = 1010 * 1010;
	}
	
	for (uint32_t a = 0; a < valuesLessThan1010.size() && (part1Ans == -1 || part2Ans == -1); a++) {
		int aVal = valuesLessThan1010[a];
		if (seen[2020 - aVal]) {
			part1Ans = aVal * (2020 - aVal);
		}
		for (uint32_t b = a + 1; b < valuesLessThan1010.size(); b++) {
			int bVal = valuesLessThan1010[b];
			int cVal = 2020 - aVal - bVal;
			if (cVal > 0 && seen[cVal]) {
				part2Ans = aVal * bVal * cVal;
				break;
			}
		}
	}
	
	ans.answerWithInts(part1Ans, part2Ans);
	return true;
}
