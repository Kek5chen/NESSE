#pragma once

class IOBus;

struct APU {
	explicit APU(IOBus *ioBus);
private:
	IOBus *mIOBus;

};