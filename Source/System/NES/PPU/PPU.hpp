#pragma once

class IOBus;

struct PPU {
	explicit PPU(IOBus *ioBus);
private:
	IOBus *mIOBus;

};