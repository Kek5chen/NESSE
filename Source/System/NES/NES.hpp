#pragma once

#include "IOBus/IOBus.hpp"
#include <filesystem>

#define CYCLES_PER_FRAME_NTSC (1789773 / 60)

struct NES {
	bool insertROM(std::filesystem::path rom);
	void run();
	void nextFrame();

	IOBus Bus;
private:
	uint16_t mCyclesLeft = 0;
	uint16_t mCyclesPerFrame = CYCLES_PER_FRAME_NTSC;
};