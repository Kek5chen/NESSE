#pragma once

class IOBus;

// https://www.nesdev.org/wiki/APU
struct APU {
	explicit APU(IOBus *ioBus);
private:
	IOBus *mIOBus;

};