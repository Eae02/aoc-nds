#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string.h>
#include <stdio.h>
#include <nds/ndstypes.h>

constexpr u32 NUM_ITERATIONS = 6;

bool solveDay17(std::string_view input, AnswerBuffer& ans) {
	const u32 inpWidth = input.find('\n');
	const u32 inpHeight = input.size() / (inpWidth + 1);
	
	const u32 width = inpWidth + NUM_ITERATIONS * 2 + 2;
	const u32 height = inpHeight + NUM_ITERATIONS * 2 + 2;
	constexpr u32 DEPTH = NUM_ITERATIONS * 2 + 4;
	
	u32 stateSize = width * height * DEPTH * DEPTH;
	std::vector<uint8_t> initialState(stateSize, 0);
	
	const u32 zStride = width * height;
	auto cellIdx = [&] (u32 x, u32 y, u32 z, u32 w) -> u32 {
		return x + y * width + z * zStride + w * zStride * DEPTH;
	};
	
	constexpr u32 STATE_OFFSET = NUM_ITERATIONS + 1;
	for (u32 y = 0; y < inpHeight; y++) {
		for (u32 x = 0; x < inpWidth; x++) {
			char c = input[y * (inpWidth + 1) + x];
			initialState[cellIdx(x + STATE_OFFSET, y + STATE_OFFSET, STATE_OFFSET, STATE_OFFSET)] = c == '#';
		}
	}
	
	std::vector<uint8_t> state = initialState;
	std::vector<uint8_t> nextState(stateSize, 0);
	auto countActive = [&] () -> u32 {
		u32 result = 0;
		for (uint8_t s : state)
			result += s;
		return result;
	};
	
	for (u32 iter = 0; iter < NUM_ITERATIONS; iter++) {
		const u32 offset = NUM_ITERATIONS - iter;
		for (u32 z = offset; z < DEPTH - offset; z++) {
			for (u32 y = offset; y < height - offset; y++) {
				for (u32 x = offset; x < width - offset; x++) {
					u32 numNeighbors = 0;
					for (int dz = -1; dz <= 1; dz++) {
						for (int dy = -1; dy <= 1; dy++) {
							for (int dx = -1; dx <= 1; dx++) {
								numNeighbors += (dz || dy || dx) && state[cellIdx(x + dx, y + dy, z + dz, STATE_OFFSET)];
								if (numNeighbors == 4)
									goto endCountNeighbors1;
							}
						}
					}
				endCountNeighbors1:
					u32 idx = cellIdx(x, y, z, STATE_OFFSET);
					nextState[idx] = (state[idx] && numNeighbors == 2) || numNeighbors == 3;
				}
			}
		}
		state.swap(nextState);
	}
	int part1Ans = countActive();
	
	memcpy(state.data(), initialState.data(), stateSize);
	memset(nextState.data(), 0, stateSize);
	for (u32 iter = 0; iter < NUM_ITERATIONS; iter++) {
		const u32 offset = NUM_ITERATIONS - iter;
		for (u32 w = offset; w < DEPTH - offset; w++) {
			for (u32 z = offset; z < DEPTH - offset; z++) {
				for (u32 y = offset; y < height - offset; y++) {
					for (u32 x = offset; x < width - offset; x++) {
						u32 numNeighbors = 0;
						for (int dw = -1; dw <= 1; dw++) {
							for (int dz = -1; dz <= 1; dz++) {
								for (int dy = -1; dy <= 1; dy++) {
									for (int dx = -1; dx <= 1; dx++) {
										numNeighbors += (dw || dz || dy || dx) && state[cellIdx(x + dx, y + dy, z + dz, w + dw)];
										if (numNeighbors == 4)
											goto endCountNeighbors2;
									}
								}
							}
						}
					endCountNeighbors2:
						u32 idx = cellIdx(x, y, z, w);
						nextState[idx] = (state[idx] && numNeighbors == 2) || numNeighbors == 3;
					}
				}
			}
		}
		state.swap(nextState);
	}
	int part2Ans = countActive();
	
	ans.answerWithInts(part1Ans, part2Ans);
	return true;
}
