#include "sol.hpp"
#include "../common.hpp"
#include "../console.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string.h>
#include <stdio.h>

struct Field {
	bool isDeparture;
	std::pair<int, int> range1;
	std::pair<int, int> range2;
};

static inline bool validForField(const Field& field, int value) {
	return
		(field.range1.first <= value && value <= field.range1.second) ||
		(field.range2.first <= value && value <= field.range2.second);
}

constexpr size_t MAX_FIELDS = 30;

static inline std::array<int, MAX_FIELDS> parseTicketLine(std::string_view line, int numFields) {
	std::array<int, MAX_FIELDS> result;
	int fieldIdx = 0;
	while (!line.empty()) {
		std::string_view part = takeCharsUntil(line, ",");
		assert(fieldIdx < numFields);
		result[fieldIdx++] = parseInt(part);
	}
	assert(fieldIdx == numFields);
	return result;
}

static inline std::pair<int, int> parseRange(std::string_view rangeStr) {
	size_t dashPos = rangeStr.find('-');
	assert(dashPos != std::string_view::npos);
	return std::make_pair(
		parseInt(rangeStr.substr(0, dashPos)),
		parseInt(rangeStr.substr(dashPos + 1))
	);
}

bool solveDay16(std::string_view input, AnswerBuffer& ans) {
	int numFields = 0;
	std::array<Field, MAX_FIELDS> fields;
	
	std::array<int, MAX_FIELDS> myTicketFields;
	
	std::vector<std::array<int, MAX_FIELDS>> otherTickets;
	
	int inpSection = 0;
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (line.empty()) continue;
		
		if (inpSection == 0) {
			if (line.starts_with("your ticket")) {
				inpSection++;
				continue;
			}
			
			size_t colPos = line.find(":");
			if (colPos == std::string_view::npos)
				continue;
			
			assertRet(numFields < (int)MAX_FIELDS);
			fields[numFields].isDeparture = line.starts_with("departure");
			
			size_t orPos = line.find("or", colPos);
			if (orPos == std::string_view::npos) {
				fields[numFields].range1 = parseRange(line.substr(colPos + 1));
				fields[numFields].range2 = { -1, -1 };
			} else {
				fields[numFields].range1 = parseRange(line.substr(colPos + 1, orPos - colPos - 1));
				fields[numFields].range2 = parseRange(line.substr(orPos + 2));
			}
			numFields++;
		} else if (inpSection == 1) {
			myTicketFields = parseTicketLine(line, numFields);
			otherTickets.reserve(std::count(input.begin(), input.end(), '\n') + 1);
			inpSection++;
		} else if (inpSection == 2 && !line.starts_with("nearby")) {
			otherTickets.push_back(parseTicketLine(line, numFields));
		}
	}
	
	std::vector<std::array<int, MAX_FIELDS>> okTickets;
	okTickets.push_back(myTicketFields);
	okTickets.reserve(otherTickets.size() + 1);
	
	int part1Ans = 0;
	for (const auto& ticket : otherTickets) {
		bool ok = true;
		for (int i = 0; i < numFields; i++) {
			bool allInvalid = true;
			for (int j = 0; j < numFields; j++) {
				if (validForField(fields[j], ticket[i])) {
					allInvalid = false;
					break;
				}
			}
			if (allInvalid) {
				part1Ans += ticket[i];
				ok = false;
			}
		}
		if (ok) {
			okTickets.push_back(ticket);
		}
	}
	
	bool isValidFieldMapping[MAX_FIELDS][MAX_FIELDS] = {};
	for (int i = 0; i < numFields; i++) {
		for (int f = 0; f < numFields; f++) {
			isValidFieldMapping[i][f] = true;
			for (const auto& ticket : okTickets) {
				if (!validForField(fields[f], ticket[i])) {
					isValidFieldMapping[i][f] = false;
					break;
				}
			}
		}
	}
	
	int fieldMapping[MAX_FIELDS];
	std::fill_n(fieldMapping, MAX_FIELDS, -1);
	bool fieldIsMappedTo[MAX_FIELDS] = {};
	
	for (int i = 0; i < numFields; i++) {
		bool anyMapped = false;
		for (int t = 0; t < numFields; t++) {
			if (fieldIsMappedTo[t]) continue;
			
			int numSources = 0;
			int sourceField = 0;
			for (int f = 0; f < numFields; f++) {
				if (fieldMapping[f] == -1 && isValidFieldMapping[f][t]) {
					numSources++;
					sourceField = f;
				}
			}
			
			if (numSources == 1) {
				fieldMapping[sourceField] = t;
				fieldIsMappedTo[t] = true;
				anyMapped = true;
				break;
			}
		}
		assertRet(anyMapped);
	}
	
	int64_t part2Ans = 1;
	for (int i = 0; i < numFields; i++) {
		if (fields[fieldMapping[i]].isDeparture) {
			part2Ans *= myTicketFields[i];
		}
	}
	
	snprintf(ans.ans1, sizeof(ans.ans1), "%d", part1Ans);
	snprintf(ans.ans2, sizeof(ans.ans2), "%lld", part2Ans);
	
	return true;
}
