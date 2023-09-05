#pragma once

#include "IOBus/IOBus.hpp"
#include <filesystem>

struct NES {
	IOBus Bus;

	bool loadROM(std::filesystem::path rom);
};