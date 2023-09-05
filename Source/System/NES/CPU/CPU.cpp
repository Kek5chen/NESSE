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
