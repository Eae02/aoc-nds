#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <vector>
#include <bitset>
#include <memory>
#include <charconv>
#include <string.h>
#include <stdio.h>

bool solveDay9(std::string_view input, AnswerBuffer& ans) {
	size_t reserveCount = std::count(input.begin(), input.end(), '\n') + 1;
	
	std::vector<uint64_t> numbers;
	numbers.reserve(reserveCount);
	
	uint64_t invalidValue = 0;
	
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (line.empty()) continue;
		
		uint64_t value;
		if (std::from_chars(line.data(), line.data() + line.size(), value).ec != std::errc()) {
			fprintf(stderr, "parse error\n");
			return false;
		}
		
		if (numbers.size() >= 25 && invalidValue == 0) {
			bool valid = false;
			for (size_t i = numbers.size() - 25; i < numbers.size() && !valid; i++) {
				if (numbers[i] > value) continue;
				uint64_t reqAtJ = value - numbers[i];
				for (size_t j = i + 1; j < numbers.size(); j++) {
					if (numbers[j] == reqAtJ) {
						valid = true;
						break;
					}
				}
			}
			if (!valid) {
				invalidValue = value;
			}
		}
		
		numbers.push_back(value);
	}
	
	size_t lp = 0;
	size_t rp = 0;
	uint64_t curSum = 0;
	uint64_t part2Ans = 0;
	while (lp < numbers.size()) {
		if (curSum < invalidValue) {
			if (rp == numbers.size()) break;
			curSum += numbers[rp];
			rp++;
		} else if (curSum > invalidValue || rp == lp + 1) {
			curSum -= numbers[lp];
			lp++;
		} else {
			uint64_t min = numbers[lp];
			uint64_t max = numbers[lp];
			for (size_t i = lp + 1; i < rp; i++) {
				min = std::min(min, numbers[i]);
				max = std::max(max, numbers[i]);
			}
			part2Ans = min + max;
			break;
		}
	}
	
	snprintf(ans.ans1, sizeof(ans.ans1), "%llu", invalidValue);
	snprintf(ans.ans2, sizeof(ans.ans2), "%llu", part2Ans);
	return true;
}
