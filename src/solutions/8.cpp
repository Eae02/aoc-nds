#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <span>
#include <assert.h>
#include <string.h>
#include <stdio.h>

enum class InstType : uint8_t {
	jmp = 0,
	acc = 1,
	nop = 2
};

struct Inst {
	InstType type;
	bool vis1;
	bool vis2[2];
	int arg;
};

static std::span<Inst> instSpan;
static int part2Acc;
bool part2Dfs(int ip, bool hasChanged) {
	if (ip == (int)instSpan.size())
		return hasChanged;
	if (ip < 0 || ip > (int)instSpan.size())
		return false;
	if (instSpan[ip].vis2[hasChanged])
		return false;
	instSpan[ip].vis2[hasChanged] = true;
	
	switch (instSpan[ip].type) {
	case InstType::jmp:
		if (!hasChanged && part2Dfs(ip + 1, true))
			return true;
		return part2Dfs(ip + instSpan[ip].arg, hasChanged);
	case InstType::nop:
		if (!hasChanged && part2Dfs(ip + instSpan[ip].arg, true))
			return true;
		return part2Dfs(ip + 1, hasChanged);
	case InstType::acc:
		bool ans = part2Dfs(ip + 1, hasChanged);
		if (ans)
			part2Acc += instSpan[ip].arg;
		return ans;
	}
	
	assert(false);
}

bool solveDay8(std::string_view input, AnswerBuffer& ans) {
	std::vector<Inst> instructions;
	instructions.reserve(std::count(input.begin(), input.end(), '\n') + 1);
	
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (line.empty()) continue;
		
		size_t spacePos = line.find(' ');
		if (spacePos == std::string_view::npos)
			continue;
		
		Inst& inst = instructions.emplace_back();
		inst.vis1 = false;
		inst.vis2[0] = false;
		inst.vis2[1] = false;
		
		std::string_view type = line.substr(0, spacePos);
		inst.arg = parseInt(line.substr(spacePos + 2));
		if (line[spacePos + 1] == '-')
			inst.arg = -inst.arg;
		
		if (type == "jmp") {
			inst.type = InstType::jmp;
		} else if (type == "acc") {
			inst.type = InstType::acc;
		} else if (type == "nop") {
			inst.type = InstType::nop;
		} else {
			printf("unknown instruction %s\n", withNullTerminator(type));
			return false;
		}
	}
	
	int part1Ans = 0;
	int ip = 0;
	while (!instructions[ip].vis1) {
		instructions[ip].vis1 = true;
		switch (instructions[ip].type) {
		case InstType::jmp:
			ip += instructions[ip].arg;
			break;
		case InstType::acc:
			part1Ans += instructions[ip].arg;
		case InstType::nop:
			ip++;
			break;
		}
	}
	
	part2Acc = 0;
	instSpan = std::span<Inst>(instructions.data(), instructions.size());
	if (!part2Dfs(0, false)) {
		return false;
	}
	
	ans.answerWithInts(part1Ans, part2Acc);
	return true;
}
