#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string.h>
#include <stdio.h>
#include <nds/ndstypes.h>

constexpr u32 MOD = 20201227;

u64 fastPow(u64 base, u64 exp) {
	if (exp == 1)
		return base;
	u64 v = fastPow(base, exp / 2);
	v = (v * v) % MOD;
	if (exp % 2) {
		v = (v * base) % MOD;
	}
	return v;
}

bool solveDay25(std::string_view input, AnswerBuffer& ans) {
	size_t newLinePos = input.find('\n');
	u32 A = parseInt(input.substr(0, newLinePos));
	u32 B = parseInt(input.substr(newLinePos + 1));
	
	int answer = -1;
	
	u32 pow7 = 1;
	for (u32 ls = 0; ls < MOD; ls++) {
		if (pow7 == A) {
			answer = fastPow(B, ls);
			break;
		}
		if (pow7 == B) {
			answer = fastPow(A, ls);
			break;
		}
		pow7 = (pow7 * 7) % MOD;
	}
	
	snprintf(ans.ans1, sizeof(ans.ans1), "%d", answer);
	strcpy(ans.ans2, "n/a");
	
	return true;
}
