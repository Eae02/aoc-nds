#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string.h>
#include <stdio.h>
#include <nds/ndstypes.h>

constexpr u32 WIDTH = 300;

bool solveDay24(std::string_view input, AnswerBuffer& ans) {
	std::bitset<WIDTH> board[2][WIDTH];
	
	int minX, minY, maxX, maxY;
	minX = minY = maxX = maxY = WIDTH / 2;
	
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (line.empty()) continue;
		
		int x = WIDTH / 2;
		int y = WIDTH / 2;
		for (int i = 0; i < (int)line.size(); i++) {
			switch (line[i]) {
			case 'e':
				x += 2;
				break;
			case 'w':
				x -= 2;
				break;
			case 'n':
				y += 1;
				x += (line[++i] == 'e') ? 1 : -1;
				break;
			case 's':
				y -= 1;
				x += (line[++i] == 'e') ? 1 : -1;
				break;
			}
		}
		
		minX = std::min(minX, x);
		minY = std::min(minY, y);
		maxX = std::max(maxX, x);
		maxY = std::max(maxY, y);
		board[0][x].flip(y);
	}
	
	int part1Ans = 0;
	for (u32 x = 0; x < WIDTH; x++) {
		part1Ans += board[0][x].count();
	}
	
	int curBoard = 0;
	for (int it = 0; it < 100; it++) {
		setSolutionProgress(it);
		
		assertRet(minX - 10 >= 0 && maxX + 10 < (int)WIDTH);
		assertRet(minY - 10 >= 0 && maxY + 10 < (int)WIDTH);
		
		int nminX, nminY, nmaxX, nmaxY;
		nminX = nminY = nmaxX = nmaxY = WIDTH / 2;
		
		for (u32 x = 0; x < WIDTH; x++) {
			board[1 - curBoard][x].reset();
		}
		
		for (int x = minX - 5; x <= maxX + 5; x++) {
			for (int y = minY - 5; y <= maxY + 5; y++) {
				int numB = 
					(int)board[curBoard][x + 2][y] + 
					(int)board[curBoard][x - 2][y] + 
					(int)board[curBoard][x + 1][y + 1] +
					(int)board[curBoard][x - 1][y + 1] +
					(int)board[curBoard][x + 1][y - 1] +
					(int)board[curBoard][x - 1][y - 1];
				
				if (numB == 2 || (board[curBoard][x][y] && numB == 1)) {
					board[1 - curBoard][x].set(y);
					nminX = std::min(nminX, x);
					nminY = std::min(nminY, y);
					nmaxX = std::max(nmaxX, x);
					nmaxY = std::max(nmaxY, y);
				}
			}
		}
		
		minX = nminX;
		minY = nminY;
		maxX = nmaxX;
		maxY = nmaxY;
		curBoard = 1 - curBoard;
	}
	
	int part2Ans = 0;
	for (u32 x = 0; x < WIDTH; x++) {
		part2Ans += board[curBoard][x].count();
	}
	
	ans.answerWithInts(part1Ans, part2Ans);
	return true;
}
