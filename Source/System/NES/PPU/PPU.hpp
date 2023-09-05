#pragma once

#include <cstdint>
#include <array>

class IOBus;

struct PPU {
	explicit PPU(IOBus *ioBus);

	template<typename T>
	void write(uint16_t dst, T &data);
	bool write(uint16_t dst, void *data, uint16_t size);
	template<typename T>
	T &read(uint16_t addr);
	template<typename T, uint16_t N>
	std::array<T, N> read(uint16_t src);
private:
	IOBus *mIOBus;

	std::array<uint8_t, 0x3FFF> mMemory;
};

#include "PPU.tpp"