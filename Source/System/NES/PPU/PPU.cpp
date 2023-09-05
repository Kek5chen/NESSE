#include "PPU.hpp"

PPU::PPU(IOBus *ioBus) : mIOBus(ioBus) {

}

bool PPU::write(uint16_t dst, void *data, uint16_t size) {
	if (dst + size > mMemory.size()) {
		ELOG("Out of Bounds Write at 0x" << std::hex << dst << " with size " << std::dec << size);
		return false;
	}

	uint8_t* byteData = static_cast<uint8_t*>(data);

	std::copy(byteData, byteData + size, mMemory.begin() + dst);

	return true;
}
