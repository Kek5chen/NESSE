#include <filesystem>
#include "NES.hpp"

bool NES::loadROM(std::filesystem::path rom) {
	if (!std::filesystem::exists(rom)) {
		return false;
	}

}
