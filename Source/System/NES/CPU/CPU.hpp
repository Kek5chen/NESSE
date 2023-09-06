#pragma once

#include <cstdint>
#include <vector>
#include "DecompilationDefines.hpp"

struct IOBus;

struct CPU {
	explicit CPU(IOBus *ioBus);

	void debugPrintState();

	bool getNegativeFlag() const;
	bool getOverflowFlag() const;
	bool getBreakFlag() const;
	bool getDecimalFlag() const;
	bool getInterruptDisableFlag() const;
	bool getZeroFlag() const;
	bool getCarryFlag() const;

	static constexpr std::string_view getMnemonic(uint8_t opcode);
	static constexpr uint8_t getOpcodeSize(uint8_t opcode);
	static constexpr uint8_t getAddressingMode(uint8_t opcode);

	std::vector<uint8_t> getInstruction();
	std::vector<uint8_t> getInstruction(uint16_t addr);
	uint8_t execute();
	void reset();

	uint8_t A;
	uint8_t X, Y;
	uint8_t P;
	uint16_t PC;
	uint8_t SP;

	struct FlagStruct {
		bool mNegative : 1;
		bool mOverflow : 1;
		bool mReserved : 1;
		bool mBreak : 1;
		bool mDecimal : 1;
		bool mInterruptDisable : 1;
		bool mZero : 1;
		bool mCarry : 1;

		FlagStruct &operator=(uint8_t newValue);
	} mFlags;

private:
	IOBus *mIOBus;
};

constexpr std::string_view CPU::getMnemonic(uint8_t opcode) {
	return std::string_view(g_mnemonics[opcode].Mnemonic.data());
}

constexpr uint8_t CPU::getOpcodeSize(uint8_t opcode) {
	uint8_t addressingMode = g_mnemonics[opcode].AddressingMode;
	if (addressingMode == IMMEDIATE || addressingMode == ZERO_PAGE || addressingMode == RELATIVE ||
			 addressingMode == ZERO_PAGE_INDEXED_X || addressingMode == ZERO_PAGE_INDEXED_Y ||
			 addressingMode == ZERO_PAGE_INDEXED_INDIRECT || addressingMode == ZERO_PAGE_INDIRECT_INDEXED_Y)
		return 2;
	else if (addressingMode == ABSOLUTE || addressingMode == ABSOLUTE_INDIRECT || addressingMode == ABSOLUTE_INDEXED_X ||
			 addressingMode == ABSOLUTE_INDEXED_Y)
		return 3;
	return 1;
}

constexpr uint8_t CPU::getAddressingMode(uint8_t opcode) {
	return g_mnemonics[opcode].AddressingMode;
}