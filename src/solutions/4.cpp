#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <assert.h>
#include <stdio.h>

enum {
	field_byr,
	field_iyr,
	field_eyr,
	field_hgt,
	field_hcl,
	field_ecl,
	field_pid,
	field_cid,
	
	num_fields
};

bool solveDay4(std::string_view input, AnswerBuffer& ans) {
	int part1Ans = 0;
	int part2Ans = 0;
	
	const char* eclValid[] = { "amb", "blu", "brn", "gry", "grn", "hzl", "oth" };
	
	while (!input.empty()) {
		std::string_view part = takeCharsUntil(input, "\n\n");
		if (isBlank(part)) continue;
		
		bool part2Ok = true;
		int numSeen = 0;
		while (true) {
			skipSpaces(part);
			size_t cpos = part.find(':');
			if (cpos == std::string_view::npos) break;
			size_t epos = std::min(part.find(' '), part.find('\n'));
			
			std::string_view key = trim(part.substr(0, cpos));
			std::string_view value = trim(part.substr(cpos + 1, epos - (cpos + 1)));
			
			if (key != "cid") {
				numSeen++;
			}
			
			if (part2Ok) {
				int intval = parseIntOr(value, -1);
				if (key == "byr" && (intval < 1920 || intval > 2002)) {
					part2Ok = false;
				}
				else if (key == "iyr" && !(intval >= 2010 && intval <= 2020)) {
					part2Ok = false;
				}
				else if (key == "eyr" && !(intval >= 2020 && intval <= 2030)) {
					part2Ok = false;
				}
				else if (key == "hgt") {
					if (!(value.ends_with("cm") && intval >= 150 && intval <= 193) && !(value.ends_with("in") && intval >= 59 && intval <= 76)) {
						part2Ok = false;
					}
				}
				else if (key == "hcl") {
					if (value.size() != 7 || value[0] != '#') {
						part2Ok = false;
					} else {
						for (char c : value.substr(1)) {
							if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f')) {
								part2Ok = false;
								break;
							}
						}
					}
				}
				else if (key == "ecl" && std::find(std::begin(eclValid), std::end(eclValid), value) == std::end(eclValid)) {
					part2Ok = false;
				}
				else if (key == "pid") {
					if (value.size() != 9) {
						part2Ok = false;
					} else {
						for (char c : value) {
							if (!(c >= '0' && c <= '9')) {
								part2Ok = false;
								break;
							}
						}
					}
				}
			}
			
			if (epos == std::string_view::npos) break;
			part.remove_prefix(epos);
		}
		
		if (numSeen == num_fields - 1) {
			part1Ans++;
			part2Ans += (int)part2Ok;
		}
	}
	
	ans.answerWithInts(part1Ans, part2Ans);
	return true;
}
