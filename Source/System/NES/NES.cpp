#include <filesystem>
#include "NES.hpp"
#include "Utils/Logger.hpp"
#include "System/NES/ROMLoader/NES/NESLoader.hpp"

bool NES::insertROM(std::filesystem::path rom) {
	ILOG("Loading " << rom << "...");
	if (!std::filesystem::exists(rom)) {
		DLOG("Rom not found!");
		return false;
	}
	DLOG("Rom found!");

	NESLoader::loadROM(rom, this);

	return true;
}

void NES::run() {
	DLOG("Starting NES...");
	Bus.mCPU.reset();
	for (int i = 0; i < 100; i++) {
		Bus.mCPU.execute();
	}
	// TODO: nextFrame(); - run nextFrame once it's safe
}

void NES::nextFrame() {
	mCyclesLeft = mCyclesPerFrame;
	while (mCyclesLeft) {
		mCyclesLeft -= Bus.mCPU.execute();
	}
}
