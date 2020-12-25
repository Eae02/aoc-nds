#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <unordered_set>
#include <string.h>
#include <stdio.h>
#include <nds/ndstypes.h>

constexpr u32 MAX_DEPTH = 20;
constexpr u32 MAX_CARDS = 64;

struct CardsList {
	u8 cards[MAX_CARDS];
	u32 firstCard = 0;
	u32 numCards = 0;
	
	void takeFrom(const CardsList& other, u32 num) {
		numCards = num;
		firstCard = 0;
		for (u32 i = 0; i < num; i++) {
			cards[i] = other.cards[(i + other.firstCard) % MAX_CARDS];
		}
	}
	
	void push(u8 card) {
		cards[(firstCard + numCards) % MAX_CARDS] = card;
		numCards++;
	}
	
	u8 pop() {
		u8 card = cards[firstCard];
		firstCard = (firstCard + 1) % MAX_CARDS;
		numCards--;
		return card;
	}
	
	u32 hash() const {
		u32 h = 0;
		u32 exp = 1;
		for (u32 i = 0; i < numCards; i++) {
			h += ((u32)cards[(firstCard + i) % MAX_CARDS] * exp);
			exp *= 50;
		}
		return h;
	}
};

int calcScore(const CardsList& cards) {
	u32 ans = 0;
	for (u32 i = 0; i < cards.numCards; i++) {
		u32 card = cards.cards[(cards.firstCard + i) % MAX_CARDS];
		ans += (cards.numCards - i) * card;
	}
	return ans;
}

bool playGame(CardsList p1, CardsList p2, bool recurse, int* scoreOut) {
	std::unordered_set<u32> seen;
	
	while (true) {
		bool alreadySeen = !seen.emplace(p1.hash()).second;
		
		if (p2.numCards == 0 || alreadySeen) {
			if (scoreOut)
				*scoreOut = calcScore(p1);
			return true;
		}
		if (p1.numCards == 0) {
			if (scoreOut)
				*scoreOut = calcScore(p2);
			return false;
		}
		
		const u8 c1 = p1.pop();
		const u8 c2 = p2.pop();
		
		bool player1Won;
		if (recurse && c1 <= p1.numCards && c2 <= p2.numCards) {
			CardsList nextP1, nextP2;
			nextP1.takeFrom(p1, c1);
			nextP2.takeFrom(p2, c2);
			player1Won = playGame(nextP1, nextP2, true, nullptr);
		} else {
			player1Won = c1 > c2;
		}
		
		if (player1Won) {
			p1.push(c1);
			p1.push(c2);
		} else {
			p2.push(c2);
			p2.push(c1);
		}
	}
}

bool solveDay22(std::string_view input, AnswerBuffer& ans) {
	std::array<CardsList, 2> inpCards;
	
	int player = -1;
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (line.empty()) continue;
		
		if (line.starts_with("Player")) {
			player++;
			continue;
		}
		
		int card = parseInt(line);
		assertRet(card >= 0 && card < (int)MAX_CARDS);
		inpCards[player].push(card);
	}
	
	int part1Ans, part2Ans;
	playGame(inpCards[0], inpCards[1], false, &part1Ans);
	playGame(inpCards[0], inpCards[1], true, &part2Ans);
	
	ans.answerWithInts(part1Ans, part2Ans);
	return true;
}
