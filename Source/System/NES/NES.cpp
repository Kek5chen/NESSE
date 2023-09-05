#include <filesystem>
#include "NES.hpp"
#include "Utils/Logger.hpp"
#include "System/NES/ROMLoader/NES/NESLoader.hpp"

bool NES::loadROM(std::filesystem::path rom) {
	ILOG("Loading " << rom << "...");
	if (!std::filesystem::exists(rom)) {
		DLOG("Rom not found!");
		return false;
	}
	DLOG("Rom found!");

	NESLoader::loadROM(rom, this);


	return true;
}
