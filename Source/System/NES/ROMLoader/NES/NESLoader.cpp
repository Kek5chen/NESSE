#include "NESLoader.hpp"
#include "NESFile.hpp"

bool NESLoader::loadROM(const std::filesystem::path &romPath, NES *nes) {
	NESFile rom(romPath);
	if (!rom.isValid())
		return false;

	if (!nes->Bus.mMemory.write(0x8000, rom.Data.PRGROM.data(), rom.Data.PRGROM.size())) {
		WLOG("Could not write PRGROM to Memory");
		return false;
	}
	if (!nes->Bus.mPPU.write(0, rom.Data.CHRROM.data(), rom.Data.CHRROM.size())) {
		WLOG("Could not write CHRROM to PPU Memory");
		return false;
	}
	ILOG("Successfully loaded ROM.");
	return true;
}
