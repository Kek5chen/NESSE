#include "Memory.hpp"
#include "Utils/Logger.hpp"

Memory::Memory(IOBus *ioBus) : mIOBus(ioBus) {

}

bool Memory::write(uint16_t dst, void *data, uint16_t size) {
	if (!data || size == 0 || dst + size > 0xFFFF) {
		ELOG("Out of Bounds Write to 0x" << std::hex << dst << " with size " << std::dec << size);
		return false;
	}

	uint8_t* byteData = static_cast<uint8_t*>(data);

	// RAM [0x0000 - 0x1FFF]
	if (dst < 0x2000) {
		uint16_t end = std::min(static_cast<uint16_t>(dst + size), static_cast<uint16_t>(0x2000));
		uint16_t len = end - dst;
		std::copy(byteData, byteData + len, mRAM.begin() + (dst % 0x800));
		size -= len;
		byteData += len;
		dst = end;
	}

	// PPU Registers [0x2000 - 0x2007]
	if (dst >= 0x2000 && dst < 0x2008) {
		uint16_t end = std::min(static_cast<uint16_t>(dst + size), static_cast<uint16_t>(0x2008));
		uint16_t len = end - dst;
		if (len <= 8) {
			std::copy(byteData, byteData + len, mPPURegisters.begin() + (dst - 0x2000));
			size -= len;
			byteData += len;
			dst = end;
		} else {
			WLOG("Failed to write to PPU Registers at 0x" << std::hex << dst);
			return false;
		}
	}

	// APU and IO Registers [0x4000 - 0x4017]
	if (dst >= 0x4000 && dst < 0x4018) {
		uint16_t end = std::min(static_cast<uint16_t>(dst + size), static_cast<uint16_t>(0x4018));
		uint16_t len = end - dst;
		if (len <= 18) {
			std::copy(byteData, byteData + len, mAPUIORegisters.begin() + (dst - 0x4000));
			size -= len;
			byteData += len;
			dst = end;
		} else {
			WLOG("Failed to write to APU and IO Registers at 0x" << std::hex << dst);
			return false;
		}
	}

	// Cartridge Space [0x4020 - 0xFFFF]
	if (dst >= 0x4020 && dst <= 0xFFFF) {
		uint16_t end = 0xFFFF;
		uint16_t len = end - dst + 1;
		if (len > size) {
			std::copy(byteData, byteData + len, mCartridgeMap.begin() + (dst - 0x4020));
		} else {
			WLOG("Failed to write to Cartridge Space at 0x" << std::hex << dst << " with size " << std::dec << size);
			return false;
		}
	}

	return true;
}
