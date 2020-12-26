#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string.h>
#include <stdio.h>
#include <nds/ndstypes.h>

//A
//A
//A
//B
//B
//B

constexpr u32 MAX_NUM = 1000000;
constexpr u32 ALLOC_BYTES = (MAX_NUM + 6) * 3;

struct NextList {
	u8 next[ALLOC_BYTES];
	
	u32 get(u32 index) const {
		return (u32)next[index * 3 + 0] | ((u32)next[index * 3 + 1] << 8) | ((u32)next[index * 3 + 2] << 16);
	}
	
	void set(u32 index, u32 value) {
		next[index * 3 + 0] = value & 0xff;
		next[index * 3 + 1] = (value >> 8) & 0xff;
		next[index * 3 + 2] = (value >> 16) & 0xff;
	}
	
	void setI(u32 index, u32 oIndex) {
		next[index * 3 + 0] = next[oIndex * 3 + 0];
		next[index * 3 + 1] = next[oIndex * 3 + 1];
		next[index * 3 + 2] = next[oIndex * 3 + 2];
	}
};

template <u32 ITER>
void simulate(u32 firstCur, u32 num, NextList& nxt) {
	u32 cur = firstCur;
	for (u32 it = 0; it < ITER; it++) {
		if constexpr (ITER > 100) {
			setSolutionProgress(it / (ITER / 100));
		}
		
		const u32 n1 = nxt.get(cur);
		const u32 n2 = nxt.get(n1);
		const u32 n3 = nxt.get(n2);
		
		u32 dst = cur ? cur - 1 : num;
		if (dst == n1 || dst == n2 || dst == n3) {
			dst = dst ? dst - 1 : num;
			if (dst == n1 || dst == n2 || dst == n3) {
				dst = dst ? dst - 1 : num;
				if (dst == n1 || dst == n2 || dst == n3) {
					dst = dst ? dst - 1 : num;
				}
			}
		}
		
		nxt.setI(cur, n3);
		nxt.setI(n3, dst);
		nxt.set(dst, n1);
		
		cur = nxt.get(cur);
	}
}

bool solveDay23(std::string_view input, AnswerBuffer& ans) {
	input = trim(input);
	
	std::unique_ptr<NextList> nxt = std::make_unique<NextList>();
	
	auto setFromInput = [&] () {
		for (u32 i = 0; i < input.size(); i++) {
			u32 v = input[i] - '1';
			u32 n = input[(i + 1) % input.size()] - '1';
			nxt->set(v, n);
		}
	};
	
	setFromInput();
	simulate<100>(input[0] - '1', input.size() - 1, *nxt);
	
	char* outPtr = ans.ans1;
	for (u32 cur = nxt->get(0); cur != 0; cur = nxt->get(cur)) {
		*outPtr = '1' + cur;
		outPtr++;
	}
	*outPtr = '\0';
	
	setFromInput();
	nxt->set(input.back() - '1', input.size());
	for (u32 i = input.size(); i < MAX_NUM - 1; i++) {
		nxt->set(i, i + 1);
	}
	nxt->set(MAX_NUM - 1, input[0] - '1');
	simulate<10000000>(input[0] - '1', MAX_NUM - 1, *nxt);
	
	u32 a1 = nxt->get(0);
	u32 aa1 = nxt->get(a1);
	snprintf(ans.ans2, sizeof(ans.ans2), "%llu", (u64)a1 * (u64)aa1);
	
	return true;
}

