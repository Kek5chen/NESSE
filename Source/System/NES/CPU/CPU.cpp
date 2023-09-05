#include <iostream>
#include "CPU.hpp"

CPU::CPU(IOBus *ioBus) {
	mIOBus = ioBus;
	mFlags = 0b00000000;
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
