#include "read_input.hpp"
#include <stdio.h>
#include <fat.h>

#ifdef EMBEDDED_INPUTS
#include "inputs.h"

static std::string_view readEmbeddedInput(int day) {
	switch (day) {
#define XM_DAY(d) case d: return std::string_view((const char*)(inp_ ## d ## _txt), inp_ ## d ## _txt_len);
#include "solved_days_xm.inl"
#undef XM_DAY
	}
	return {};
}
#else
static std::string_view readEmbeddedInput(int day) { return { }; }
#endif

#include <memory>

static bool couldInitializeLibFat = false;
static std::unique_ptr<char[]> inputBuffer;

static std::string_view readFileInput(int day) {
	inputBuffer.reset();
	
	char filePath[64];
	snprintf(filePath, sizeof(filePath), "inp/%d.txt", day);
	FILE* fp = fopen(filePath, "rb");
	if (fp == nullptr) {
		fprintf(stderr, "error opening %s\n", filePath);
		return {};
	}
	
	fseek(fp, 0, SEEK_END);
	size_t inputLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	inputBuffer = std::make_unique<char[]>(inputLen);
	if (fread(inputBuffer.get(), 1, inputLen, fp) != inputLen) {
		fprintf(stderr, "error reading %s\n", filePath);
		return {};
	}
	
	return std::string_view(inputBuffer.get(), inputLen);
}

void initInputReading() {
	couldInitializeLibFat = fatInitDefault();
	if (!couldInitializeLibFat) {
		fprintf(stderr, "could not init libfat\n");
	}
}

std::string_view readInput(int day) {
	if (couldInitializeLibFat) {
		std::string_view fileInput = readFileInput(day);
		if (!fileInput.empty()) 
			return fileInput;
	}
	return readEmbeddedInput(day);
}
