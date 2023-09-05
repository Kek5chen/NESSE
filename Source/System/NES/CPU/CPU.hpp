#pragma once

#include <cstdint>

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
private:
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

	IOBus *mIOBus;
};