#pragma once

#include "../IOBus/IOBus.hpp"
#include <array>
#include <functional>

struct Opcode {
	std::function<void(IOBus *ioBus, uint8_t byte1, uint8_t byte2)> function;
	uint8_t cycles;
};

extern const std::array<Opcode, 256> g_opcodes;