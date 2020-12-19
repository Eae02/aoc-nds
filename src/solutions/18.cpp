#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string.h>
#include <stdio.h>
#include <nds/ndstypes.h>

std::pair<int64_t, int64_t> eval(std::string_view str) {
	constexpr u32 MAX_DEPTH = 128;
	
	int64_t currentVal1[MAX_DEPTH];
	int64_t currentVal2A[MAX_DEPTH];
	int64_t currentVal2M[MAX_DEPTH];
	bool pendingMul[MAX_DEPTH];
	
	int depth = 0;
	auto initNewDepth = [&] () {
		currentVal1[depth] = 0;
		currentVal2A[depth] = 0;
		currentVal2M[depth] = 1;
		pendingMul[depth] = false;
	};
	initNewDepth();
	
	auto applyValue = [&] (int64_t value1, int64_t value2) {
		if (pendingMul[depth]) {
			currentVal1[depth] *= value1;
		} else {
			currentVal1[depth] += value1;
		}
		currentVal2A[depth] += value2;
	};
	auto applyPendingMultiply = [&] {
		if (currentVal2A[depth] != 0) {
			currentVal2M[depth] *= currentVal2A[depth];
			currentVal2A[depth] = 0;
		}
	};
	
	for (const char c : str) {
		switch (c) {
		case (int)'0' ... (int)'9':
			applyValue(c - '0', c - '0');
			break;
		case '(':
			depth++;
			assert(depth < (int)MAX_DEPTH);
			initNewDepth();
			break;
		case ')': {
			applyPendingMultiply();
			int64_t val1 = currentVal1[depth];
			int64_t val2 = currentVal2M[depth];
			depth--;
			assert(depth >= 0);
			applyValue(val1, val2);
			break;
		}
		case '+':
			pendingMul[depth] = false;
			break;
		case '*':
			applyPendingMultiply();
			pendingMul[depth] = true;
			break;
		default:
			break;
		}
	}
	
	applyPendingMultiply();
	return std::make_pair(currentVal1[depth], currentVal2M[depth]);
}

bool solveDay18(std::string_view input, AnswerBuffer& ans) {
	int64_t part1Ans = 0;
	int64_t part2Ans = 0;
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (!line.empty()) {
			auto [p1, p2] = eval(line);
			part1Ans += p1;
			part2Ans += p2;
		}
	}
	
	snprintf(ans.ans1, sizeof(ans.ans1), "%lld", part1Ans);
	snprintf(ans.ans2, sizeof(ans.ans2), "%lld", part2Ans);
	return true;
}
