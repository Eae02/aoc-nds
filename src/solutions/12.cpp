#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <assert.h>
#include <string.h>
#include <stdio.h>

bool solveDay12(std::string_view input, AnswerBuffer& ans) {
	int part1X = 0, part1Y = 0, part2X = 0, part2Y = 0, dirX = 0, dirY = 1, wpX = 10, wpY = 1;
	
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (line.empty()) continue;
		
		int arg = parseInt(line.substr(1));
		switch (line[0]) {
		case 'N':
			part1Y += arg;
			wpY += arg;
			break;
		case 'S':
			part1Y -= arg;
			wpY -= arg;
			break;
		case 'E':
			part1X += arg;
			wpX += arg;
			break;
		case 'W':
			part1X -= arg;
			wpX -= arg;
			break;
		case 'F':
			part2X += wpX * arg;
			part2Y += wpY * arg;
			part1X += dirX * arg;
			part1Y += dirY * arg;
			break;
		case 'R':
			arg = -arg;
		case 'L':
			arg = ((arg / 90) + 4) % 4;
			for (int i = 0; i < arg; i++) {
				std::swap(wpX, wpY);
				wpX = -wpX;
				std::swap(dirX, dirY);
				dirX = -dirX;
			}
			break;
		}
	}
	
	ans.answerWithInts(abs(part1X) + abs(part1Y), abs(part2X) + abs(part2Y));
	return true;
}
