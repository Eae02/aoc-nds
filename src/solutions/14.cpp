#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string.h>
#include <stdio.h>

static constexpr size_t MASK_LEN = 36;

struct Write {
	char mask[MASK_LEN];
	uint64_t dst;
	uint64_t val;
};

struct MemoryWrite {
	uint64_t addr;
	uint64_t val;
};

template <bool ReportProgress>
uint64_t calcMemorySum(std::vector<MemoryWrite>& memory) {
	constexpr uint32_t HASH_BITS = MASK_LEN / 2;
	
	std::vector<int> firstInGroup(1 << HASH_BITS, -1);
	std::vector<int> nextInSameGroup(memory.size());
	
	uint64_t ans = 0;
	for (int i = (int)memory.size() - 1; i >= 0; i--) {
		if constexpr (ReportProgress) {
			setSolutionProgress(50 + (memory.size() - i) * 50 / memory.size());
		}
		
		const uint32_t group =
			((uint32_t)memory[i].addr & (uint32_t)((1 << HASH_BITS) - 1)) ^
			((uint32_t)memory[i].addr >> (uint32_t)HASH_BITS);
		
		bool blocked = false;
		for (int j = firstInGroup[group]; j != -1; j = nextInSameGroup[j]) {
			if (memory[i].addr == memory[j].addr) {
				blocked = true;
				break;
			}
		}
		if (!blocked) {
			ans += memory[i].val;
			nextInSameGroup[i] = firstInGroup[group];
			firstInGroup[group] = i;
		}
	}
	
	return ans;
}

std::vector<MemoryWrite>* part2MemoryGlobal;

void writeAll(const Write& write, uint32_t idx, uint64_t dst) {
	if (idx == MASK_LEN) {
		part2MemoryGlobal->push_back({ dst, write.val });
		return;
	}
	
	const uint64_t bm = (uint64_t)1 << (uint64_t)idx;
	if (write.mask[idx] == '0') {
		writeAll(write, idx + 1, dst | (write.dst & bm));
		return;
	}
	if (write.mask[idx] == '1') {
		writeAll(write, idx + 1, dst | bm);
		return;
	}
	
	writeAll(write, idx + 1, dst | bm);
	writeAll(write, idx + 1, dst);
}

bool solveDay14(std::string_view input, AnswerBuffer& ans) {
	char mask[MASK_LEN];
	
	size_t reserveCountWrites = std::count(input.begin(), input.end(), '\n') + 1;
	std::vector<Write> part2Writes;
	part2Writes.reserve(reserveCountWrites);
	
	std::vector<MemoryWrite> part1Memory;
	part1Memory.reserve(reserveCountWrites);
	
	size_t part2WritesMult = 0;
	size_t part2ReserveCount = 0;
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		
		if (line.starts_with("mask")) {
			memcpy(mask, &line[line.size() - MASK_LEN], MASK_LEN);
			std::reverse(std::begin(mask), std::end(mask));
			part2WritesMult = 1 << (size_t)std::count(std::begin(mask), std::end(mask), 'X');
		} else if (line.starts_with("mem")) {
			Write& write = part2Writes.emplace_back();
			write.val = parseInt(line.substr(line.find('=') + 1));
			write.dst = parseInt(line.substr(line.find('[') + 1));
			memcpy(write.mask, mask, MASK_LEN);
			
			uint64_t valPart1 = write.val;
			for (size_t i = 0; i < MASK_LEN; i++) {
				uint64_t maskHere = (uint64_t)1 << (uint64_t)i;
				if (mask[i] == '1') {
					valPart1 |= maskHere;
				} else if (mask[i] == '0') {
					valPart1 &= ~maskHere;
				}
			}
			part1Memory.push_back({ write.dst, valPart1 });
			
			part2ReserveCount += part2WritesMult;
		}
	}
	assert(part2Writes.size() <= reserveCountWrites);
	assert(part1Memory.size() <= reserveCountWrites);
	
	uint64_t part1Ans = calcMemorySum<false>(part1Memory);
	
	std::vector<MemoryWrite> part2Memory;
	part2Memory.reserve(part2ReserveCount);
	part2MemoryGlobal = &part2Memory;
	for (size_t i = 0; i < part2Writes.size(); i++) {
		setSolutionProgress(i * 50 / part2Writes.size());
		writeAll(part2Writes[i], 0, 0);
	}
	
	assert(part2Memory.size() <= part2ReserveCount);
	uint64_t part2Ans = calcMemorySum<true>(part2Memory);
	
	snprintf(ans.ans1, sizeof(ans.ans1), "%lld", part1Ans);
	snprintf(ans.ans2, sizeof(ans.ans2), "%lld", part2Ans);
	return true;
}
