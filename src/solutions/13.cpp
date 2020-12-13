#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

int64_t extEuclid(int64_t a, int64_t b, int64_t& x, int64_t& y) {
	if (b) {
		int64_t d = extEuclid(b, a % b, y, x);
		return y -= a / b * x, d;
	}
	return x = 1, y = 0, a;
}



bool solveDay13(std::string_view input, AnswerBuffer& ans) {
	size_t firstNewline = input.find('\n');
	int curTime = parseInt(input.substr(0, firstNewline));
	
	std::pair<int, int> ansPart1(INT_MAX, INT_MAX);
	std::vector<std::pair<int, int>> eqsPart2;
	eqsPart2.reserve(std::count(input.begin(), input.end(), ',') + 1);
	
	int64_t p = 1;
	
	std::string_view line2 = input.substr(firstNewline + 1);
	int busIdx = 0;
	while (!line2.empty()) {
		std::string_view part = takeCharsUntil(line2, ",");
		if (part != "x") {
			int t = parseInt(part);
			int elapsedTime = curTime % t;
			int remTime = elapsedTime ? t - elapsedTime : 0;
			ansPart1 = std::min(ansPart1, std::make_pair(remTime, t));
			eqsPart2.emplace_back(busIdx, t);
			p *= (int64_t)t;
		}
		busIdx++;
	}
	
	int64_t ansPart2 = 0;
	for (const std::pair<int, int>& e : eqsPart2) {
		int64_t pp = p / (int64_t)e.second, ppi, y;
		extEuclid(pp, e.second, ppi, y);
		ansPart2 = (ansPart2 + (int64_t)e.first * ppi * pp) % p;
	}
	
	snprintf(ans.ans1, sizeof(ans.ans1), "%d", ansPart1.first * ansPart1.second);
	snprintf(ans.ans2, sizeof(ans.ans2), "%lld", -ansPart2);
	return true;
}
