#pragma once

#include <string_view>

std::string_view readFileInput(int day);
std::string_view readEmbeddedInput(int day);

using ReadInputCallback = std::string_view(*)(int);
