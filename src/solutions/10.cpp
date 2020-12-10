#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <assert.h>
#include <string.h>
#include <stdio.h>

bool solveDay10(std::string_view input, AnswerBuffer& ans) {
	std::vector<int> numbers;
	numbers.reserve(std::count(input.begin(), input.end(), '\n') + 1);
	
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (!line.empty()) {
			numbers.push_back(parseInt(line));
		}
	}
	
	numbers.push_back(0);
	std::sort(numbers.begin(), numbers.end());
	numbers.push_back(numbers.back() + 3);
	
	int numDiff1 = 0;
	int numDiff3 = 0;
	for (size_t i = 1; i < numbers.size(); i++) {
		int diff = numbers[i] - numbers[i - 1];
		if (diff == 3) numDiff3++;
		if (diff == 1) numDiff1++;
	}
	
	std::vector<uint64_t> ways(numbers.size());
	ways.back() = 1;
	for (int i = (int)numbers.size() - 2; i >= 0; i--) {
		ways[i] = 0;
		for (int j = i + 1; j < (int)numbers.size() && numbers[j] - numbers[i] <= 3; j++) {
			ways[i] += ways[j];
		}
	}
	
	snprintf(ans.ans1, sizeof(ans.ans1), "%d", numDiff1 * numDiff3);
	snprintf(ans.ans2, sizeof(ans.ans2), "%llu", ways[0]);
	return true;
}
