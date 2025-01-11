#include <iostream>
#include "CPU.hpp"
#include "../IOBus/IOBus.hpp"
#include "OpcodeFunctions.hpp"

CPU::CPU(IOBus *ioBus) {
	mIOBus = ioBus;
	mFlags = 0b00000000;
	SP = 0xFF;
}

bool CPU::getNegativeFlag() const {
	return mFlags.mNegative;
}

bool CPU::getOverflowFlag() const {
	return mFlags.mOverflow;
}

bool CPU::getBreakFlag() const {
	return mFlags.mBreak;
}

bool CPU::getDecimalFlag() const {
	return mFlags.mDecimal;
}

bool CPU::getInterruptDisableFlag() const {
	return mFlags.mInterruptDisable;
}

bool CPU::getZeroFlag() const {
	return mFlags.mZero;
}

bool CPU::getCarryFlag() const {
	return mFlags.mCarry;
}

void CPU::debugPrintState() {
#ifdef DEBUG
	std::cout << "---- CPU State ----" << std::endl;
	std::cout << "A: " << std::hex << +A << std::endl;
	std::cout << "X: " << std::hex << +X << std::endl;
	std::cout << "Y: " << std::hex << +Y << std::endl;
	std::cout << "P: " << std::hex << +P << std::endl;
	std::cout << "PC: " << std::hex << PC << std::endl;
	std::cout << "SP: " << std::hex << +SP << std::endl;

	std::cout << "Flags:" << std::endl;
	std::cout << "Negative: " << (mFlags.mNegative ? "1" : "0") << std::endl;
	std::cout << "Overflow: " << (mFlags.mOverflow ? "1" : "0") << std::endl;
	std::cout << "Reserved: " << (mFlags.mReserved ? "1" : "0") << std::endl;
	std::cout << "Break: " << (mFlags.mBreak ? "1" : "0") << std::endl;
	std::cout << "Decimal: " << (mFlags.mDecimal ? "1" : "0") << std::endl;
	std::cout << "InterruptDisable: " << (mFlags.mInterruptDisable ? "1" : "0") << std::endl;
	std::cout << "Zero: " << (mFlags.mZero ? "1" : "0") << std::endl;
	std::cout << "Carry: " << (mFlags.mCarry ? "1" : "0") << std::endl;

	std::cout << "-------------------" << std::endl;
#endif
}

std::vector<uint8_t> CPU::getInstruction(uint16_t addr) {
	std::vector<uint8_t> instruction;
	auto opcode = mIOBus->mMemory.read<uint8_t>(addr);
	instruction.push_back(opcode);
	uint8_t instructionSize = getOpcodeSize(opcode);
	if (instructionSize == 0)
		return instruction;

	if (instructionSize != 1)
		for (uint8_t i = 1; i < instructionSize; ++i)
			instruction.push_back(mIOBus->mMemory.read<uint8_t>(addr + i));

	return instruction;
}

std::vector<uint8_t> CPU::getInstruction() {
	return getInstruction(PC);
}

uint8_t CPU::execute() {
	auto curInstruction = getInstruction();

	auto opcode = curInstruction[0];
	auto byte1 = curInstruction.size() > 1 ? curInstruction[1] : 0;
	auto byte2 = curInstruction.size() > 2 ? curInstruction[2] : 0;

	PC += curInstruction.size();

	g_opcodes[opcode].function(mIOBus, byte1, byte2);
	return g_opcodes[opcode].cycles;
}

void CPU::reset() {
	A = 0;
	X = 0;
	Y = 0;
	P = 0;
	SP = 0xFF;
	PC = mIOBus->mMemory.read<uint16_t>(0xFFFC);
	mFlags = 0b00000000;
}

CPU::FlagStruct &CPU::FlagStruct::operator=(uint8_t newValue) {
	mNegative = newValue & 1;
	mOverflow = (newValue >> 1) & 1;
	mBreak = (newValue >> 3) & 1;
	mDecimal = (newValue >> 4) & 1;
	mInterruptDisable = (newValue >> 5) & 1;
	mZero = (newValue >> 6) & 1;
	mCarry = (newValue >> 7) & 1;

	return *this;
}
