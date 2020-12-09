#pragma once

#include <string_view>

void initInputReading();
std::string_view readInput(int day);

using ReadInputCallback = std::string_view(*)(int);
