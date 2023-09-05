#pragma once

#include "../Memory/Memory.hpp"
#include "../CPU/CPU.hpp"
#include "../APU/APU.hpp"
#include "../PPU/PPU.hpp"

struct IOBus {
	CPU mCPU;
	Memory mMemory;
	PPU mPPU;
	APU mAPU;
};