#include "common.hpp"

#include <string.h>
#include <stdio.h>
#include <nds.h>
#include <charconv>

std::string_view takeCharsUntil(std::string_view& str, std::string_view delim) {
	size_t pos = str.find(delim);
	std::string_view result;
	if (pos == std::string_view::npos) {
		result = str;
		str = {};
	} else {
		result = str.substr(0, pos);
		str = str.substr(std::min(pos + delim.size(), str.size()));
	}
	return result;
}

void skipSpaces(std::string_view& str) {
	while (!str.empty() && isspace(str.front())) {
		str.remove_prefix(1);
	}
}

std::string_view trim(std::string_view str) {
	while (!str.empty() && isspace(str.front())) {
		str.remove_prefix(1);
	}
	while (!str.empty() && isspace(str.back())) {
		str.remove_suffix(1);
	}
	return str;
}

bool isBlank(std::string_view sv) {
	for (char c : sv) {
		if (!isspace(c))
			return false;
	}
	return true;
}

static char withNullTermBuffer[64];

const char* withNullTerminator(std::string_view sv) {
	size_t len = std::min(sv.size(), sizeof(withNullTermBuffer) - 1);
	memcpy(withNullTermBuffer, sv.data(), len);
	withNullTermBuffer[len] = '\0';
	return withNullTermBuffer;
}

void printStringView(std::string_view sv) {
	printf("psv: %s\n", withNullTerminator(sv));
}

int parseInt(std::string_view str) {
	skipSpaces(str);
	int value;
	if (std::from_chars(str.data(), str.data() + str.size(), value).ec != std::errc()) {
		printf("parseInt called with %s!\n", withNullTerminator(str));
		return 0;
	}
	return value;
}

int parseIntOr(std::string_view str, int def) {
	skipSpaces(str);
	int value;
	if (std::from_chars(str.data(), str.data() + str.size(), value).ec != std::errc())
		return def;
	return value;
}

uint32_t ticksToMS(uint32_t ticks) {
	return ((uint64_t)ticks * 1000UL + (uint64_t)BUS_CLOCK / 2) / (uint64_t)BUS_CLOCK;
}
