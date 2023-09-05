#pragma once

#include <cstdint>
#include <array>

class IOBus;

struct Memory {
	explicit Memory(IOBus *ioBus);
	// write();
	// read();
private:
	IOBus *mIOBus;

	std::array<uint8_t, 0x0800>		mRAM;				// 0x0000 - 0x07FF mirrored to 0x1FFF
	std::array<uint8_t, 0x8>		mPPURegisters;		// 0x2000 - 0x2007
	std::array<uint8_t, 0x18>		mAPUIORegisters;	// 0x4000 - 0x4017
	std::array<uint8_t, 0xBFDF>		mCartridgeMap;		// 0x4020 - 0xFFFF

};