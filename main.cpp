#include <iostream>
#include "Source/System/NES/NES.hpp"

int main() {
	NES nesEmu;

	nesEmu.insertROM("Balloon Fight (USA).nes");
	nesEmu.Bus.mMemory.debugPrintDisassembly();
	nesEmu.run();
	return 0;
}