#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string.h>
#include <stdio.h>
#include <nds/ndstypes.h>

constexpr u32 MAX_STR_LEN = 100;
constexpr u32 MAX_RULES = 200;

struct __attribute__((packed)) Rule {
	u8 sub1L;
	u8 sub1R;
	u8 sub2L;
	u8 sub2R;
};

static inline void parseSubRulePair(std::string_view s, u8& outL, u8& outR) {
	s = trim(s);
	size_t spacePos = s.find(' ');
	if (spacePos == std::string_view::npos) {
		outL = parseInt(s);
		outR = UINT8_MAX;
	} else {
		outL = parseInt(s.substr(0, spacePos));
		outR = parseInt(s.substr(spacePos + 1));
	}
}

static std::bitset<MAX_RULES> toposortVisited;
static u32* toposortOutput;

void rulesTopoSort(const Rule* rules, const char* rulesFixedChar, u32 rule) {
	if (rule == UINT8_MAX || toposortVisited[rule]) return;
	toposortVisited.set(rule);
	
	if (!rulesFixedChar[rule]) {
		rulesTopoSort(rules, rulesFixedChar, rules[rule].sub1L);
		rulesTopoSort(rules, rulesFixedChar, rules[rule].sub1R);
		rulesTopoSort(rules, rulesFixedChar, rules[rule].sub2L);
		rulesTopoSort(rules, rulesFixedChar, rules[rule].sub2R);
	}
	
	*toposortOutput = rule;
	toposortOutput++;
}

struct DpTable {
	int16_t data[MAX_RULES][MAX_STR_LEN + 1];
};

bool solveDay19(std::string_view input, AnswerBuffer& ans) {
	Rule rules[MAX_RULES];
	char rulesFixedChar[MAX_RULES] = {};
	
	std::unique_ptr<DpTable> dp = std::make_unique<DpTable>();
	
	std::vector<std::string_view> stringsToTest;
	stringsToTest.reserve(std::count(input.begin(), input.end(), '\n'));
	
	int part1Ans = 0;
	int part2Ans = 0;
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (line.empty()) continue;
		
		size_t colonPos = line.find(':');
		
		if (colonPos == std::string_view::npos) {
			stringsToTest.push_back(trim(line));
		} else {
			int ruleIdx = parseInt(line.substr(0, colonPos));
			std::string_view afterColon = trim(line.substr(colonPos + 1));
			assert(!afterColon.empty());
			
			if (afterColon[0] == '\"') {
				rulesFixedChar[ruleIdx] = afterColon[1];
			} else {
				size_t orPos = afterColon.find('|');
				if (orPos != std::string_view::npos) {
					parseSubRulePair(afterColon.substr(0, orPos), rules[ruleIdx].sub1L, rules[ruleIdx].sub1R);
					parseSubRulePair(afterColon.substr(orPos + 1), rules[ruleIdx].sub2L, rules[ruleIdx].sub2R);
				} else {
					parseSubRulePair(afterColon, rules[ruleIdx].sub1L, rules[ruleIdx].sub1R);
					rules[ruleIdx].sub2L = UINT8_MAX;
					rules[ruleIdx].sub2R = UINT8_MAX;
				}
			}
		}
	}
	
	u32 rulesToposorted[MAX_RULES];
	toposortVisited = {};
	toposortOutput = rulesToposorted;
	rulesTopoSort(rules, rulesFixedChar, 0);
	
	const u32 numRules = toposortOutput - rulesToposorted;
	u32 numBaseRules = 0;
	while (rulesFixedChar[rulesToposorted[numBaseRules]]) {
		numBaseRules++;
	}
	
	assertRet(rules[0].sub1L == 8 && rules[0].sub1R == 11);
	
	memset(dp->data, -1, sizeof(dp->data));
	for (u32 s = 0; s < stringsToTest.size(); s++) {
		setSolutionProgress(s * 100 / stringsToTest.size());
		
		for (u32 b = 0; b < numBaseRules; b++) {
			u32 rule = rulesToposorted[b];
			for (u32 i = 0; i < stringsToTest[s].size(); i++) {
				dp->data[rule][i] = (stringsToTest[s][i] == rulesFixedChar[rule]) ? (i + 1) : -1;
			}
		}
		
		std::for_each_n(rulesToposorted + numBaseRules, numRules - numBaseRules, [&] (u32 rule) {
			const Rule ruleInfo = rules[rule];
			
			for (u32 st = 0; st < stringsToTest[s].size(); st++) {
				auto checkSubRules = [&] (u8 subL, u8 subR) -> int16_t {
					int16_t edL = dp->data[subL][st];
					if (subR == UINT8_MAX || edL == -1)
						return edL;
					return dp->data[subR][edL];
				};
				
				dp->data[rule][st] = checkSubRules(ruleInfo.sub1L, ruleInfo.sub1R);
				if (ruleInfo.sub2L != UINT8_MAX && dp->data[rule][st] == -1) {
					dp->data[rule][st] = checkSubRules(ruleInfo.sub2L, ruleInfo.sub2R);
				}
			}
		});
		
		if (dp->data[0][0] == (int16_t)stringsToTest[s].size()) {
			part1Ans++;
			part2Ans++;
		} else {
			int16_t endPos42 = dp->data[42][0];
			for (int num42 = 0; endPos42 != -1; num42++) {
				int num31 = 0;
				int16_t endPos31 = dp->data[31][endPos42];
				while (endPos31 != -1 && endPos31 != (int16_t)stringsToTest[s].size()) {
					num31++;
					endPos31 = dp->data[31][endPos31];
				}
				if (endPos31 == (int16_t)stringsToTest[s].size() && num31 < num42) {
					part2Ans++;
					break;
				}
				endPos42 = dp->data[42][endPos42];
			}
		}
	}
	
	ans.answerWithInts(part1Ans, part2Ans);
	return true;
}
