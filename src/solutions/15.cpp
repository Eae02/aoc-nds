#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string.h>
#include <stdio.h>
#include <nds.h>

static constexpr bool ENABLE_PART_2 = false;

bool solveDay15(std::string_view input, AnswerBuffer& ans) {
	constexpr int ITERATIONS_PART_1 = 2020;
	constexpr int ITERATIONS_PART_2 = 30000000;
	
	constexpr size_t NUM_IN_RAM = 800000;
	std::unique_ptr<int[]> lastSeen = std::make_unique<int[]>(NUM_IN_RAM);
	memset(lastSeen.get(), -1, NUM_IN_RAM * sizeof(int));
	
	int it = 0;
	
	while (!input.empty()) {
		std::string_view part = takeCharsUntil(input, ",");
		if (!part.empty()) {
			int num = parseInt(part);
			lastSeen[num] = it++;
		}
	}
	
	int prevNumberLastSeen = -1;
	int num = 0;
	for (; it < ITERATIONS_PART_1; it++) {
		if (prevNumberLastSeen != -1) {
			num = it - prevNumberLastSeen - 1;
		} else {
			num = 0;
		}
		prevNumberLastSeen = lastSeen[num];
		lastSeen[num] = it;
	}
	snprintf(ans.ans1, sizeof(ans.ans1), "%d", num);
	
	if (ENABLE_PART_2) {
		if (FILE* fp = fopen("aoc15_tmp", "w+")) {
			constexpr size_t NUM_ON_DISK = ITERATIONS_PART_2 - NUM_IN_RAM;
			
			int neg1Buffer[1024];
			memset(neg1Buffer, -1, sizeof(neg1Buffer));
			for (size_t i = 0; i < NUM_ON_DISK; i += std::size(neg1Buffer)) {
				size_t bytesWritten = fwrite(neg1Buffer, 1, sizeof(neg1Buffer), fp);
				assert(bytesWritten == sizeof(neg1Buffer));
			}
			
			for (; it < ITERATIONS_PART_2; it++) {
				setSolutionProgress((uint32_t)it * (uint32_t)100 / (uint32_t)ITERATIONS_PART_2);
				
				if (prevNumberLastSeen != -1) {
					num = it - prevNumberLastSeen - 1;
				} else {
					num = 0;
				}
				
				if (num < (int)NUM_IN_RAM) {
					prevNumberLastSeen = lastSeen[num];
					lastSeen[num] = it;
				} else {
					fseek(fp, (num - NUM_IN_RAM) * sizeof(int), SEEK_SET);
					fread(&prevNumberLastSeen, 1, sizeof(int), fp);
					fseek(fp, -sizeof(int), SEEK_CUR);
					fwrite(&it, 1, sizeof(int), fp);
				}
			}
			snprintf(ans.ans2, sizeof(ans.ans2), "%d", num);
			
			fclose(fp);
		} else {
			strcpy(ans.ans2, "no file io");
		}
	} else {
		strcpy(ans.ans2, "disabled");
	}
	
	return true;
}
