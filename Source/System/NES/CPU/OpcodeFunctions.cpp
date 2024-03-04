#include "OpcodeFunctions.hpp"

static void updateValue(IOBus *ioBus, uint8_t &field, uint8_t value) {
	field = value;
	ioBus->mCPU.mFlags.mZero = (value == 0);
	ioBus->mCPU.mFlags.mNegative = ((int8_t)value < 0);
}

void BRK_IMPLIED(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	ioBus->mMemory.write<uint8_t>(ioBus->mCPU.SP--, ioBus->mCPU.PC >> 8);
	ioBus->mMemory.write<uint8_t>(ioBus->mCPU.SP--, ioBus->mCPU.PC & 0xFF);
	ioBus->mMemory.write<uint8_t>(ioBus->mCPU.SP--, ioBus->mCPU.P);
	ioBus->mCPU.mFlags.mBreak = true;
	ioBus->mCPU.mFlags.mInterruptDisable = true;
	ioBus->mCPU.PC = ioBus->mMemory.read<uint16_t>(0xFFFE);
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

void NOP_IMMEDIATE(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	// Do nothing
}



void INS_NULLFUNC(IOBus *ioBus, uint8_t byte1, uint8_t byte2) {
	DLOG("Game called NULLFUNC with 0x" << std::hex << +byte1 << " and 0x" << +byte2);
}

// TODO: Maybe we have to care about if operations pass a page boundary because it'd add a cycle
const std::array<Opcode, 256> g_opcodes = std::to_array<Opcode>({
	////////////// 00 /////////////
	{BRK_IMPLIED, 7},		// 0x00
	{INS_NULLFUNC, 0},		// 0x01
	{INS_NULLFUNC, 0},		// 0x02
	{INS_NULLFUNC, 0},		// 0x03
	{NOP_IMMEDIATE, 3},		// 0x04
	{INS_NULLFUNC, 0},		// 0x05
	{INS_NULLFUNC, 0},		// 0x06
	{INS_NULLFUNC, 0},		// 0x07
	{INS_NULLFUNC, 0},		// 0x08
	{INS_NULLFUNC, 0},		// 0x09
	{INS_NULLFUNC, 0},		// 0x0A
	{INS_NULLFUNC, 0},		// 0x0B
	{NOP_IMMEDIATE, 4},		// 0x0C
	{INS_NULLFUNC, 0},		// 0x0D
	{INS_NULLFUNC, 0},		// 0x0E
	{INS_NULLFUNC, 0},		// 0x0F
	////////////// 10 /////////////
	{BPL_RELATIVE, 1},		// 0x10
	{INS_NULLFUNC, 0},		// 0x11
	{INS_NULLFUNC, 0},		// 0x12
	{INS_NULLFUNC, 0},		// 0x13
	{NOP_IMMEDIATE, 4},		// 0x14
	{INS_NULLFUNC, 0},		// 0x15
	{INS_NULLFUNC, 0},		// 0x16
	{INS_NULLFUNC, 0},		// 0x17
	{INS_NULLFUNC, 0},		// 0x18
	{INS_NULLFUNC, 0},		// 0x19
	{NOP_IMMEDIATE, 2},		// 0x1A
	{INS_NULLFUNC, 0},		// 0x1B
	{NOP_IMMEDIATE, 4},		// 0x1C
	{INS_NULLFUNC, 0},		// 0x1D
	{INS_NULLFUNC, 0},		// 0x1E
	{INS_NULLFUNC, 0},		// 0x1F
	////////////// 20 /////////////
	{JSR_ABSOLUTE, 1},		// 0x20
	{INS_NULLFUNC, 0},		// 0x21
	{INS_NULLFUNC, 0},		// 0x22
	{INS_NULLFUNC, 0},		// 0x23
	{INS_NULLFUNC, 0},		// 0x24
	{INS_NULLFUNC, 0},		// 0x25
	{INS_NULLFUNC, 0},		// 0x26
	{INS_NULLFUNC, 0},		// 0x27
	{INS_NULLFUNC, 0},		// 0x28
	{INS_NULLFUNC, 0},		// 0x29
	{INS_NULLFUNC, 0},		// 0x2A
	{INS_NULLFUNC, 0},		// 0x2B
	{INS_NULLFUNC, 0},		// 0x2C
	{INS_NULLFUNC, 0},		// 0x2D
	{INS_NULLFUNC, 0},		// 0x2E
	{INS_NULLFUNC, 0},		// 0x2F
	////////////// 30 /////////////
	{BMI_RELATIVE, 1},		// 0x30
	{INS_NULLFUNC, 0},		// 0x31
	{INS_NULLFUNC, 0},		// 0x32
	{INS_NULLFUNC, 0},		// 0x33
	{NOP_IMMEDIATE, 4},		// 0x34
	{INS_NULLFUNC, 0},		// 0x35
	{INS_NULLFUNC, 0},		// 0x36
	{INS_NULLFUNC, 0},		// 0x37
	{INS_NULLFUNC, 0},		// 0x38
	{INS_NULLFUNC, 0},		// 0x39
	{NOP_IMMEDIATE, 2},		// 0x3A
	{INS_NULLFUNC, 0},		// 0x3B
	{NOP_IMMEDIATE, 4},		// 0x3C
	{INS_NULLFUNC, 0},		// 0x3D
	{INS_NULLFUNC, 0},		// 0x3E
	{INS_NULLFUNC, 0},		// 0x3F
	////////////// 40 /////////////
	{INS_NULLFUNC, 1},		// 0x40
	{INS_NULLFUNC, 0},		// 0x41
	{INS_NULLFUNC, 0},		// 0x42
	{INS_NULLFUNC, 0},		// 0x43
	{NOP_IMMEDIATE, 3},		// 0x44
	{INS_NULLFUNC, 0},		// 0x45
	{INS_NULLFUNC, 0},		// 0x46
	{INS_NULLFUNC, 0},		// 0x47
	{INS_NULLFUNC, 0},		// 0x48
	{INS_NULLFUNC, 0},		// 0x49
	{INS_NULLFUNC, 0},		// 0x4A
	{INS_NULLFUNC, 0},		// 0x4B
	{INS_NULLFUNC, 0},		// 0x4C
	{INS_NULLFUNC, 0},		// 0x4D
	{INS_NULLFUNC, 0},		// 0x4E
	{INS_NULLFUNC, 0},		// 0x4F
	////////////// 50 /////////////
	{INS_NULLFUNC, 1},		// 0x50
	{INS_NULLFUNC, 0},		// 0x51
	{INS_NULLFUNC, 0},		// 0x52
	{INS_NULLFUNC, 0},		// 0x53
	{NOP_IMMEDIATE, 4},		// 0x54
	{INS_NULLFUNC, 0},		// 0x55
	{INS_NULLFUNC, 0},		// 0x56
	{INS_NULLFUNC, 0},		// 0x57
	{INS_NULLFUNC, 0},		// 0x58
	{INS_NULLFUNC, 0},		// 0x59
	{NOP_IMMEDIATE, 2},		// 0x5A
	{INS_NULLFUNC, 0},		// 0x5B
	{NOP_IMMEDIATE, 4},		// 0x5C
	{INS_NULLFUNC, 0},		// 0x5D
	{INS_NULLFUNC, 0},		// 0x5E
	{INS_NULLFUNC, 0},		// 0x5F
	////////////// 60 /////////////
	{INS_NULLFUNC, 1},		// 0x60
	{INS_NULLFUNC, 0},		// 0x61
	{INS_NULLFUNC, 0},		// 0x62
	{INS_NULLFUNC, 0},		// 0x63
	{NOP_IMMEDIATE, 3},		// 0x64
	{INS_NULLFUNC, 0},		// 0x65
	{INS_NULLFUNC, 0},		// 0x66
	{INS_NULLFUNC, 0},		// 0x67
	{INS_NULLFUNC, 0},		// 0x68
	{INS_NULLFUNC, 0},		// 0x69
	{INS_NULLFUNC, 0},		// 0x6A
	{INS_NULLFUNC, 0},		// 0x6B
	{INS_NULLFUNC, 0},		// 0x6C
	{INS_NULLFUNC, 0},		// 0x6D
	{INS_NULLFUNC, 0},		// 0x6E
	{INS_NULLFUNC, 0},		// 0x6F
	////////////// 70 /////////////
	{INS_NULLFUNC, 1},		// 0x70
	{INS_NULLFUNC, 0},		// 0x71
	{INS_NULLFUNC, 0},		// 0x72
	{INS_NULLFUNC, 0},		// 0x73
	{NOP_IMMEDIATE, 4},		// 0x74
	{INS_NULLFUNC, 0},		// 0x75
	{INS_NULLFUNC, 0},		// 0x76
	{INS_NULLFUNC, 0},		// 0x77
	{SEI_IMPLIED, 1},		// 0x78
	{INS_NULLFUNC, 0},		// 0x79
	{NOP_IMMEDIATE, 2},		// 0x7A
	{INS_NULLFUNC, 0},		// 0x7B
	{NOP_IMMEDIATE, 4},		// 0x7C
	{INS_NULLFUNC, 0},		// 0x7D
	{INS_NULLFUNC, 0},		// 0x7E
	{INS_NULLFUNC, 0},		// 0x7F
	////////////// 80 /////////////
	{NOP_IMMEDIATE, 2},		// 0x80
	{INS_NULLFUNC, 0},		// 0x81
	{NOP_IMMEDIATE, 2},		// 0x82
	{INS_NULLFUNC, 0},		// 0x83
	{INS_NULLFUNC, 0},		// 0x84
	{INS_NULLFUNC, 0},		// 0x85
	{INS_NULLFUNC, 0},		// 0x86
	{INS_NULLFUNC, 0},		// 0x87
	{INS_NULLFUNC, 1},		// 0x88
	{NOP_IMMEDIATE, 2},		// 0x89
	{TXA_IMPLIED, 1},		// 0x8A
	{INS_NULLFUNC, 0},		// 0x8B
	{INS_NULLFUNC, 0},		// 0x8C
	{STA_ABSOLUTE, 1},		// 0x8D
	{INS_NULLFUNC, 0},		// 0x8E
	{INS_NULLFUNC, 0},		// 0x8F
	////////////// 90 /////////////
	{INS_NULLFUNC, 1},		// 0x90
	{INS_NULLFUNC, 0},		// 0x91
	{INS_NULLFUNC, 0},		// 0x92
	{INS_NULLFUNC, 0},		// 0x93
	{INS_NULLFUNC, 0},		// 0x94
	{INS_NULLFUNC, 0},		// 0x95
	{INS_NULLFUNC, 0},		// 0x96
	{INS_NULLFUNC, 0},		// 0x97
	{INS_NULLFUNC, 1},		// 0x98
	{INS_NULLFUNC, 0},		// 0x99
	{TXS_IMPLIED, 1},		// 0x9A
	{INS_NULLFUNC, 0},		// 0x9B
	{INS_NULLFUNC, 0},		// 0x9C
	{STA_ABSOLUTE_IX, 1},	// 0x9D
	{INS_NULLFUNC, 0},		// 0x9E
	{INS_NULLFUNC, 0},		// 0x9F
	////////////// A0 /////////////
	{INS_NULLFUNC, 0},		// 0xA0
	{INS_NULLFUNC, 0},		// 0xA1
	{LDX_IMMEDIATE, 1},		// 0xA2
	{INS_NULLFUNC, 0},		// 0xA3
	{INS_NULLFUNC, 0},		// 0xA4
	{INS_NULLFUNC, 0},		// 0xA5
	{INS_NULLFUNC, 0},		// 0xA6
	{INS_NULLFUNC, 0},		// 0xA7
	{INS_NULLFUNC, 0},		// 0xA8
	{LDA_IMMEDIATE, 1},		// 0xA9
	{INS_NULLFUNC, 0},		// 0xAA
	{INS_NULLFUNC, 0},		// 0xAB
	{INS_NULLFUNC, 0},		// 0xAC
	{LDA_ABSOLUTE, 1},		// 0xAD
	{INS_NULLFUNC, 0},		// 0xAE
	{INS_NULLFUNC, 0},		// 0xAF
	////////////// B0 /////////////
	{INS_NULLFUNC, 0},		// 0xB0
	{INS_NULLFUNC, 0},		// 0xB1
	{INS_NULLFUNC, 0},		// 0xB2
	{INS_NULLFUNC, 0},		// 0xB3
	{INS_NULLFUNC, 0},		// 0xB4
	{INS_NULLFUNC, 0},		// 0xB5
	{INS_NULLFUNC, 0},		// 0xB6
	{INS_NULLFUNC, 0},		// 0xB7
	{INS_NULLFUNC, 0},		// 0xB8
	{INS_NULLFUNC, 0},		// 0xB9
	{INS_NULLFUNC, 0},		// 0xBA
	{INS_NULLFUNC, 0},		// 0xBB
	{INS_NULLFUNC, 0},		// 0xBC
	{LDA_ABSOLUTE_IX, 1},	// 0xBD
	{INS_NULLFUNC, 0},		// 0xBE
	{INS_NULLFUNC, 0},		// 0xBF
	////////////// C0 /////////////
	{INS_NULLFUNC, 0},		// 0xC0
	{INS_NULLFUNC, 0},		// 0xC1
	{NOP_IMMEDIATE, 2},		// 0xC2
	{INS_NULLFUNC, 0},		// 0xC3
	{INS_NULLFUNC, 0},		// 0xC4
	{INS_NULLFUNC, 0},		// 0xC5
	{INS_NULLFUNC, 0},		// 0xC6
	{INS_NULLFUNC, 0},		// 0xC7
	{INS_NULLFUNC, 0},		// 0xC8
	{INS_NULLFUNC, 0},		// 0xC9
	{DEX_IMPLIED, 1},		// 0xCA
	{INS_NULLFUNC, 0},		// 0xCB
	{INS_NULLFUNC, 0},		// 0xCC
	{INS_NULLFUNC, 0},		// 0xCD
	{INS_NULLFUNC, 0},		// 0xCE
	{INS_NULLFUNC, 0},		// 0xCF
	////////////// D0 /////////////
	{BNE_RELATIVE, 1},		// 0xD0
	{INS_NULLFUNC, 0},		// 0xD1
	{INS_NULLFUNC, 0},		// 0xD2
	{INS_NULLFUNC, 0},		// 0xD3
	{NOP_IMMEDIATE, 4},		// 0xD4
	{INS_NULLFUNC, 0},		// 0xD5
	{INS_NULLFUNC, 0},		// 0xD6
	{INS_NULLFUNC, 0},		// 0xD7
	{CLD_IMPLIED, 1},		// 0xD8
	{INS_NULLFUNC, 0},		// 0xD9
	{NOP_IMMEDIATE, 2},		// 0xDA
	{INS_NULLFUNC, 0},		// 0xDB
	{NOP_IMMEDIATE, 4},		// 0xDC
	{CMP_ABSOLUTE_IX, 1},	// 0xDD
	{INS_NULLFUNC, 0},		// 0xDE
	{INS_NULLFUNC, 0},		// 0xDF
	////////////// E0 /////////////
	{INS_NULLFUNC, 0},		// 0xE0
	{INS_NULLFUNC, 0},		// 0xE1
	{NOP_IMMEDIATE, 2},		// 0xE2
	{INS_NULLFUNC, 0},		// 0xE3
	{INS_NULLFUNC, 0},		// 0xE4
	{INS_NULLFUNC, 0},		// 0xE5
	{INS_NULLFUNC, 0},		// 0xE6
	{INS_NULLFUNC, 0},		// 0xE7
	{INX_IMPLIED, 1},		// 0xE8
	{INS_NULLFUNC, 0},		// 0xE9
	{NOP_IMMEDIATE, 2},		// 0xEA
	{INS_NULLFUNC, 0},		// 0xEB
	{INS_NULLFUNC, 0},		// 0xEC
	{INS_NULLFUNC, 0},		// 0xED
	{INS_NULLFUNC, 0},		// 0xEE
	{INS_NULLFUNC, 0},		// 0xEF
	////////////// F0 /////////////
	{INS_NULLFUNC, 0},		// 0xF0
	{INS_NULLFUNC, 0},		// 0xF1
	{INS_NULLFUNC, 0},		// 0xF2
	{INS_NULLFUNC, 0},		// 0xF3
	{NOP_IMMEDIATE, 4},		// 0xF4
	{INS_NULLFUNC, 0},		// 0xF5
	{INS_NULLFUNC, 0},		// 0xF6
	{INS_NULLFUNC, 0},		// 0xF7
	{INS_NULLFUNC, 0},		// 0xF8
	{INS_NULLFUNC, 0},		// 0xF9
	{NOP_IMMEDIATE, 2},		// 0xFA
	{INS_NULLFUNC, 0},		// 0xFB
	{NOP_IMMEDIATE, 4},		// 0xFC
	{INS_NULLFUNC, 0},		// 0xFD
	{INS_NULLFUNC, 0},		// 0xFE
	{INS_NULLFUNC, 0},		// 0xFF
});

static_assert(g_opcodes.size() == 256);
