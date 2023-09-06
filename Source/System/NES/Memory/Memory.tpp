
template<typename T>
void Memory::write(uint16_t dst, const T &data) {
	write(dst, (void *) &data, sizeof(T));
}

template<typename T, uint16_t N>
std::array<T, N> Memory::read(uint16_t src) {
	std::array<T, N> result = {};
	uint16_t size = N;

	auto dest = reinterpret_cast<uint8_t*>(result.data());

	// RAM [0x0000 - 0x1FFF]
	if (src < 0x2000) {
		uint16_t end = std::min(static_cast<uint16_t>(src + size), static_cast<uint16_t>(0x2000));
		uint16_t len = end - src;
		std::copy(mRAM.begin() + (src % 0x800), mRAM.begin() + (src % 0x800) + len, dest);
		size -= len;
		dest += len;
		src = end;
	}

	// PPU Registers [0x2000 - 0x2007]
	if (src >= 0x2000 && src < 0x2008) {
		uint16_t end = std::min(static_cast<uint16_t>(src + size), static_cast<uint16_t>(0x2008));
		uint16_t len = end - src;
		if (len <= 8) {
			std::copy(mPPURegisters.begin() + (src - 0x2000), mPPURegisters.begin() + (src - 0x2000) + len, dest);
			size -= len;
			dest += len;
			src = end;
		} else {
			WLOG("Failed to read from PPU Registers at 0x" << std::hex << src);
			return {};
		}
	}

	// APU and IO Registers [0x4000 - 0x4017]
	if (src >= 0x4000 && src < 0x4018) {
		uint16_t end = std::min(static_cast<uint16_t>(src + size), static_cast<uint16_t>(0x4018));
		uint16_t len = end - src;
		if (len <= 18) {
			std::copy(mAPUIORegisters.begin() + (src - 0x4000), mAPUIORegisters.begin() + (src - 0x4000) + len, dest);
			size -= len;
			dest += len;
			src = end;
		} else {
			WLOG("Failed to read from APU and IO Registers at 0x" << std::hex << src);
			return {};
		}
	}

	// Cartridge Space [0x4020 - 0xFFFF]
	if (src >= 0x4020 && src < 0xFFFF) {
		uint16_t end = 0xFFFF;
		uint16_t len = end - src + 1;
		if (len > size) {
			std::copy(mCartridgeMap.begin() + (src - 0x4020), mCartridgeMap.begin() + (src - 0x4020) + size, dest);
		} else {
			WLOG("Failed to read from Cartridge Space at 0x" << std::hex << src);
			return {};
		}
	}

	return result;
}


template<typename T>
T Memory::read(uint16_t addr) {
	std::array<T, 1> value = read<T, 1>(addr);
	return *value.data();
}