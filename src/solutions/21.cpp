#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <unordered_map>
#include <memory>
#include <string.h>
#include <stdio.h>
#include <nds/ndstypes.h>

constexpr size_t MAX_I = 200;
constexpr size_t MAX_A = 8;

bool solveDay21(std::string_view input, AnswerBuffer& ans) {
	std::unordered_map<std::string_view, u32> iMap;
	std::vector<std::string_view> iList;
	std::vector<std::string_view> aList;
	std::vector<int> iOccCount;
	iOccCount.reserve(MAX_I);
	iMap.reserve(MAX_I);
	aList.reserve(MAX_A);
	
	bool incompatible[MAX_I][MAX_A] = {};
	
	int ansPart1 = 0;
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (line.empty()) continue;
		
		const size_t openParenPos = line.find('(');
		const size_t nextSpacePos = line.find(' ', openParenPos);
		const size_t closeParenPos = line.find(')', nextSpacePos);
		assertRet(openParenPos != std::string_view::npos && nextSpacePos != std::string_view::npos && closeParenPos != std::string_view::npos);
		
		bool lineHasI[MAX_I] = {};
		
		std::string_view iStr = line.substr(0, openParenPos);
		while (!iStr.empty()) {
			const std::string_view iName = takeCharsUntil(iStr, " ");
			u32 iid = iMap.size();
			auto it = iMap.find(iName);
			if (it != iMap.end()) {
				iid = it->second;
			} else {
				iMap.emplace(iName, iid);
				iList.push_back(iName);
				iOccCount.push_back(0);
				assertRet(iList.size() <= MAX_I);
			}
			iOccCount[iid]++;
			ansPart1++;
			lineHasI[iid] = true;
		}
		
		std::string_view aStr = line.substr(nextSpacePos + 1, closeParenPos - nextSpacePos - 1);
		while (!aStr.empty()) {
			const std::string_view aName = trim(takeCharsUntil(aStr, ","));
			const size_t idx = std::find(aList.begin(), aList.end(), aName) - aList.begin();
			if (idx == aList.size()) {
				aList.push_back(aName);
				assertRet(aList.size() <= MAX_A);
			}
			
			for (u32 i = 0; i < MAX_I; i++) {
				if (!lineHasI[i]) {
					incompatible[i][idx] = true;
				}
			}
		}
	}
	
	const u32 maxABM = 1U << (u32)aList.size();
	std::bitset<1 << MAX_A> dp[MAX_I + 1];
	dp[iList.size()][maxABM - 1] = true;
	for (int i = (int)iList.size() - 1; i >= 0; i--) {
		for (u32 bm = 0; bm < maxABM; bm++) {
			dp[i][bm] = dp[i + 1][bm];
			for (u32 a = 0; a < MAX_A; a++) {
				const u32 nMask = (1 << a) | bm;
				if (nMask != bm && dp[i + 1][nMask] && !incompatible[i][a]) {
					dp[i][bm] = true;
					break;
				}
			}
		}
	}
	
	std::vector<std::pair<std::string_view, std::string_view>> part2Vec;
	part2Vec.reserve(aList.size());
	u32 curBM = 0;
	for (u32 i = 0; i < iList.size(); i++) {
		if (dp[i + 1][curBM])
			continue;
		bool foundNext = false;
		for (u32 a = 0; a < MAX_A; a++) {
			const u32 nMask = (1 << a) | curBM;
			if (nMask != curBM && dp[i + 1][nMask] && !incompatible[i][a]) {
				part2Vec.emplace_back(aList[a], iList[i]);
				ansPart1 -= iOccCount[i];
				curBM = nMask;
				foundNext = true;
				break;
			}
		}
		assertRet(foundNext);
	}
	
	snprintf(ans.ans1, sizeof(ans.ans1), "%d", ansPart1);
	
	std::sort(part2Vec.begin(), part2Vec.end());
	size_t outPos = 0;
	for (size_t i = 0; i < part2Vec.size(); i++) {
		if (i != 0 && outPos < sizeof(ans.ans2) - 1) {
			ans.ans2[outPos++] = ',';
		}
		for (char c : part2Vec[i].second) {
			if (outPos < sizeof(ans.ans2) - 1) {
				ans.ans2[outPos++] = c;
			}
		}
	}
	ans.ans2[outPos] = '\0';
	
	return true;
}
