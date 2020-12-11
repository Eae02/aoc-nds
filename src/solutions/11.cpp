#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <assert.h>
#include <string.h>
#include <stdio.h>

bool solveDay11(std::string_view input, AnswerBuffer& ans) {
	const int width = input.find('\n');
	const int height = input.size() / (width + 1);
	
	std::vector<uint8_t> isSeat(width * height);
	std::vector<std::array<int, 8>> nextSeeingSeatIdx(width * height);
	std::vector<std::array<int, 8>> nextImmediateSeatIdx(width * height);
	
	int idx = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			char c = input[y * (width + 1) + x];
			isSeat[idx] = c != '.';
			idx++;
		}
	}
	
	auto getIndexOrNeg1 = [&] (int x, int y) {
		if (x < 0 || y < 0 || x >= width || y >= height)
			return -1;
		return x + y * width;
	};
	
	idx = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int nextSeatIdxImm = 0;
			int nextSeatIdxSee = 0;
			for (int dy = -1; dy <= 1; dy++) {
				for (int dx = -1; dx <= 1; dx++) {
					if (dx == 0 && dy == 0) continue;
					
					int nx = x + dx;
					int ny = y + dy;
					
					int ni = getIndexOrNeg1(nx, ny);
					if (ni != -1) {
						nextImmediateSeatIdx[idx][nextSeatIdxImm++] = ni;
					}
					
					while (nx >= 0 && ny >= 0 && nx < width && ny < height && !isSeat[ny * width + nx]) {
						nx += dx;
						ny += dy;
					}
					ni = getIndexOrNeg1(nx, ny);
					if (ni != -1) {
						nextSeeingSeatIdx[idx][nextSeatIdxSee++] = ni;
					}
				}
			}
			while (nextSeatIdxImm < 8) {
				nextImmediateSeatIdx[idx][nextSeatIdxImm++] = -1;
			}
			while (nextSeatIdxSee < 8) {
				nextSeeingSeatIdx[idx][nextSeatIdxSee++] = -1;
			}
			idx++;
		}
	}
	
	std::vector<uint8_t> isOccP1(width * height, 0);
	std::vector<uint8_t> wasOccP1(width * height, 0);
	std::vector<uint8_t> isOccP2(width * height, 0);
	std::vector<uint8_t> wasOccP2(width * height, 0);
	bool simulateAgain = true;
	while (simulateAgain) {
		idx = 0;
		simulateAgain = false;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++, idx++) {
				if (!isSeat[idx])
					continue;
				auto anyOccP1 = [&] () {
					for (int ni : nextImmediateSeatIdx[idx]) {
						if (ni == -1) break;
						if (wasOccP1[ni])
							return true;
					}
					return false;
				};
				auto anyOccP2 = [&] () {
					for (int ni : nextSeeingSeatIdx[idx]) {
						if (ni == -1) break;
						if (wasOccP2[ni])
							return true;
					}
					return false;
				};
				auto atLeast4OccupiedP1 = [&] () {
					int c = 0;
					for (int ni : nextImmediateSeatIdx[idx]) {
						if (ni == -1) break;
						if (wasOccP1[ni] && (++c) == 4) {
							return true;
						}
					}
					return false;
				};
				auto atLeast5OccupiedP2 = [&] () {
					int c = 0;
					for (int ni : nextSeeingSeatIdx[idx]) {
						if (ni == -1) break;
						if (wasOccP2[ni] && (++c) == 5) {
							return true;
						}
					}
					return false;
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
