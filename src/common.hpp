#pragma once

#include <string_view>

#define assert(x) if (!(x)) { fprintf(stderr, "error %s @%s:%d\n", #x, __FILE__, __LINE__); std::abort(); }
#define assertRet(x) if (!(x)) { fprintf(stderr, "error %s @%s:%d\n", #x, __FILE__, __LINE__); return false; }

std::string_view takeCharsUntil(std::string_view& str, std::string_view delim);

void skipSpaces(std::string_view& str);

std::string_view trim(std::string_view str);
bool isBlank(std::string_view sv);

const char* withNullTerminator(std::string_view sv);

void printStringView(std::string_view sv);

int parseInt(std::string_view str);
int parseIntOr(std::string_view str, int def);

uint32_t ticksToMS(uint32_t ticks);
