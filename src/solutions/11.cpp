#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string.h>
#include <stdio.h>

bool solveDay11(std::string_view input, AnswerBuffer& ans) {
	const int oriWidth = input.find('\n');
	const int oriHeight = input.size() / (oriWidth + 1);
	const int width = oriWidth + 2;
	const int height = oriHeight + 2;
	
	std::vector<uint8_t> isSeat(width * height, 1);
	std::vector<std::array<int, 8>> nextSeeingSeatIdx(width * height);
	
	int idx = width + 1;
	for (int y = 0; y < oriHeight; y++, idx += 2) {
		for (int x = 0; x < oriWidth; x++, idx++) {
			char c = input[y * (oriWidth + 1) + x];
			isSeat[idx] = c != '.';
		}
	}
	
	auto getIndex = [&] (int x, int y) {
		return x + y * width;
	};
	
	idx = width + 1;
	for (int y = 1; y <= oriHeight; y++, idx += 2) {
		for (int x = 1; x <= oriWidth; x++, idx++) {
			int nextSeatIdxIdx = 0;
			for (int dy = -1; dy <= 1; dy++) {
				for (int dx = -1; dx <= 1; dx++) {
					if (dx == 0 && dy == 0) continue;
					
					int nx = x + dx;
					int ny = y + dy;
					while (!isSeat[getIndex(nx, ny)]) {
						nx += dx;
						ny += dy;
					}
					nextSeeingSeatIdx[idx][nextSeatIdxIdx++] = getIndex(nx, ny);
				}
			}
		}
	}
	
	std::vector<uint8_t> isOccP1(width * height, 0);
	std::vector<uint8_t> wasOccP1(width * height, 0);
	std::vector<uint8_t> isOccP2(width * height, 0);
	std::vector<uint8_t> wasOccP2(width * height, 0);
	bool simulateAgain = true;
	while (simulateAgain) {
		idx = width + 1;
		simulateAgain = false;
		for (int y = 1; y <= oriHeight; y++, idx += 2) {
			for (int x = 1; x <= oriWidth; x++, idx++) {
				if (!isSeat[idx])
					continue;
				
				int pRowIdx = idx - width;
				int nRowIdx = idx + width;
				auto anyOccP1 = [&] () {
					return
						wasOccP1[idx - 1] || wasOccP1[idx + 1] || 
						wasOccP1[pRowIdx - 1] || wasOccP1[pRowIdx] || wasOccP1[pRowIdx + 1] ||
						wasOccP1[nRowIdx - 1] || wasOccP1[nRowIdx] || wasOccP1[nRowIdx + 1];
				};
				auto anyOccP2 = [&] () {
					for (int ni : nextSeeingSeatIdx[idx]) {
						if (wasOccP2[ni])
							return true;
					}
					return false;
				};
				auto atLeast4OccupiedP1 = [&] () {
					int c = wasOccP1[idx - 1] + wasOccP1[idx + 1] + 
						wasOccP1[pRowIdx - 1] + wasOccP1[pRowIdx] + wasOccP1[pRowIdx + 1] +
						wasOccP1[nRowIdx - 1] + wasOccP1[nRowIdx] + wasOccP1[nRowIdx + 1];
					return c >= 4;
				};
				auto atLeast5OccupiedP2 = [&] () {
					int c = 0;
					for (int ni : nextSeeingSeatIdx[idx]) {
						c += wasOccP2[ni];
					}
					return c >= 5;
				};
				
				if (!wasOccP1[idx]) {
					if (!anyOccP1()) {
						isOccP1[idx] = true;
						simulateAgain = true;
					} else {
						isOccP1[idx] = wasOccP1[idx];
					}
				} else if (atLeast4OccupiedP1()) {
					isOccP1[idx] = false;
					simulateAgain = true;
				} else {
					isOccP1[idx] = wasOccP1[idx];
				}
				
				if (!wasOccP2[idx]) {
					if (!anyOccP2()) {
						isOccP2[idx] = true;
						simulateAgain = true;
					} else {
						isOccP2[idx] = wasOccP2[idx];
					}
				} else if (atLeast5OccupiedP2()) {
					isOccP2[idx] = false;
					simulateAgain = true;
				} else {
					isOccP2[idx] = wasOccP2[idx];
				}
			}
		}
		isOccP1.swap(wasOccP1);
		isOccP2.swap(wasOccP2);
	}
	
	int part1Ans = 0;
	int part2Ans = 0;
	for (int i = width * height - 1; i >= 0; i--) {
		if (wasOccP1[i]) part1Ans++;
		if (wasOccP2[i]) part2Ans++;
	}
	
	ans.answerWithInts(part1Ans, part2Ans);
	return true;
}
