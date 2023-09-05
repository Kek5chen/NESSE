#include <filesystem>
#include "NES.hpp"
#include "Utils/Logger.hpp"

bool NES::loadROM(std::filesystem::path rom) {
	ILOG("Loading " << rom << "...");
	if (!std::filesystem::exists(rom)) {
		DLOG("Rom not found!");
		return false;
	}
	DLOG("Rom found!");

}
