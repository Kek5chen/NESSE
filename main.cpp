#include <iostream>
#include "Source/System/NES/NES.hpp"

int main() {
	NES nesEmu;

	nesEmu.loadROM("Balloon Fight (USA).nes");
	nesEmu.loadROM("King of Fighters 99.nes");
	return 0;
}