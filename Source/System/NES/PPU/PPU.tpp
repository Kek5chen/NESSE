
#include <cstdint>
#include "Utils/Logger.hpp"

template<typename T>
void PPU::write(uint16_t dst, T &data) {
	write(dst, &data, sizeof(T));
}

template<typename T, uint16_t N>
std::array<T, N> PPU::read(uint16_t src) {
	std::array<T, N> result = {};

	if (src + N > mMemory.size()) {
		ELOG("Out of Bounds Write at 0x" << std::hex << src << " with size " << std::dec << N);
		return {};
	}

	std::copy(mMemory.begin() + src, mMemory.begin() + src + N, result.data());

	return result;
}

template<typename T>
T &PPU::read(uint16_t addr) {
	std::array<T, sizeof(T)> value = read<T, sizeof(T)>(addr, sizeof(T));
	return *value.data();
}