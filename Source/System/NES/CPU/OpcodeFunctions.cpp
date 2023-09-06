#include "OpcodeFunctions.hpp"

static void updateValue(IOBus *ioBus, uint8_t &field, uint8_t value) {
	field = value;
	ioBus->mCPU.mFlags.mZero = (value == 0);
	ioBus->mCPU.mFlags.mNegative = ((int8_t)value < 0);
}

void LDA_IMMEDIATE(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	updateValue(ioBus, ioBus->mCPU.A, byte1);
}

void STA_ABSOLUTE(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	uint16_t addr = byte1 | (byte2 << 8);
	ioBus->mMemory.write(addr, ioBus->mCPU.A);
}

void LDA_ABSOLUTE(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	uint16_t addr = byte1 | (byte2 << 8);
	auto value = ioBus->mMemory.read<int8_t>(addr);
	updateValue(ioBus, ioBus->mCPU.A, value);
}

void BPL_RELATIVE(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	if (!ioBus->mCPU.getNegativeFlag())
		ioBus->mCPU.PC += (int8_t)byte1;
}

void BMI_RELATIVE(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	if (ioBus->mCPU.getNegativeFlag())
		ioBus->mCPU.PC += (int8_t)byte1;
}

void SEI_IMPLIED(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	ioBus->mCPU.mFlags.mInterruptDisable = true;
}

void CLD_IMPLIED(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	ioBus->mCPU.mFlags.mDecimal = false;
}

void LDX_IMMEDIATE(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	updateValue(ioBus, ioBus->mCPU.X, byte1);
}

void TXS_IMPLIED(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	updateValue(ioBus, ioBus->mCPU.SP, ioBus->mCPU.X);
}

void TXA_IMPLIED(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	updateValue(ioBus, ioBus->mCPU.A, ioBus->mCPU.X);
}

void INX_IMPLIED(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	updateValue(ioBus, ioBus->mCPU.X, ioBus->mCPU.X + 1);
}

void DEX_IMPLIED(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	updateValue(ioBus, ioBus->mCPU.X, ioBus->mCPU.X - 1);
}

void BNE_RELATIVE(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	if (!ioBus->mCPU.getZeroFlag())
		ioBus->mCPU.PC += (int8_t)byte1;
}

void LDA_ABSOLUTE_IX(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	uint16_t addr = byte1 | (byte2 << 8);
	updateValue(ioBus, ioBus->mCPU.A, ioBus->mMemory.read<uint8_t>(addr) + ioBus->mCPU.X);
}

void CMP_ABSOLUTE_IX(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	uint16_t addr = byte1 | (byte2 << 8);
	auto value = ioBus->mMemory.read<uint8_t>(addr) + ioBus->mCPU.X;
	auto otherValue = ioBus->mCPU.A;
	auto subtraction = (int8_t)(value - otherValue);
	ioBus->mCPU.mFlags.mCarry = (value >= otherValue);
	ioBus->mCPU.mFlags.mZero = !subtraction;
	ioBus->mCPU.mFlags.mNegative = (subtraction < 0);
}

void STA_ABSOLUTE_IX(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	uint16_t addr = byte1 | (byte2 << 8);
	ioBus->mMemory.write(addr + ioBus->mCPU.X, ioBus->mCPU.A);
}

void JSR_ABSOLUTE(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	uint16_t addr = byte1 | (byte2 << 8);
	uint16_t returnAddr = ioBus->mCPU.PC - 2;
	ioBus->mMemory.write<uint8_t>(ioBus->mCPU.SP--, returnAddr >> 8);
	ioBus->mMemory.write<uint8_t>(ioBus->mCPU.SP--, returnAddr & 0xFF);
	ioBus->mCPU.PC = addr;
}



void INS_NULLFUNC(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	DLOG("Game called NULLFUNC with " << std::hex << byte1 << " and " << byte2);
}

const Opcode g_opcodes[256] = {
	{BPL_RELATIVE, 1},		// 0x10
	{JSR_ABSOLUTE, 1},		// 0x20
	{BMI_RELATIVE, 1},		// 0x30
	{SEI_IMPLIED, 1},		// 0x78
	{TXA_IMPLIED, 1},		// 0x8A
	{STA_ABSOLUTE, 1},		// 0x8D
	{TXS_IMPLIED, 1},		// 0x9A
	{STA_ABSOLUTE_IX, 1},	// 0x9D
	{LDX_IMMEDIATE, 1},		// 0xA2
	{LDA_IMMEDIATE, 1},		// 0xA9
	{LDA_ABSOLUTE, 1},		// 0xAD
	{LDA_ABSOLUTE_IX, 1},	// 0xBD
	{DEX_IMPLIED, 1},		// 0xCA
	{BNE_RELATIVE, 1},		// 0xD0
	{CLD_IMPLIED, 1},		// 0xD8
	{CMP_ABSOLUTE_IX, 1},	// 0xDD
	{INX_IMPLIED, 1},		// 0xE8
};