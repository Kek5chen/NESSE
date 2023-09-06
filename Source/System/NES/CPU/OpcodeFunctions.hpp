#pragma once

#include "../IOBus/IOBus.hpp"
#include <array>

typedef void (*t_opcode_function)(IOBus *ioBus, uint8_t byte1, uint8_t byte2);

struct Opcode {
	t_opcode_function function;
	uint8_t cycles;
};

extern const std::array<Opcode, 256> g_opcodes;