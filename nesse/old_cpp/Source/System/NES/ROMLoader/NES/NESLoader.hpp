#pragma once

#include <filesystem>
#include "System/NES/NES.hpp"

struct NESLoader {
	static bool loadROM(const std::filesystem::path &romPath, NES *nes);
};