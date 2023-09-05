#pragma once

class IOBus;

class APU {
	explicit APU(IOBus *ioBus);
private:
	IOBus *mIOBus;

};