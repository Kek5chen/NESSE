#pragma once

#include <cstdint>
#include <vector>
#include <filesystem>

struct NESFileHeader {
	uint32_t MagicNumber;
	uint8_t PRGSize;
	uint8_t CHRSize;
	struct {
		uint8_t MapperLower : 4;
		uint8_t IgnoreMirrorControl : 1;
		uint8_t HasByteTrainer : 1;
		uint8_t HasBatteryBackedPRGRAM : 1;
		uint8_t MirrorDirection : 1; // 0: Horizontal; 1: Vertical
	} Flags6;
	struct {
		uint8_t MapperUpper : 4;
		uint8_t NESVersion : 2;
		uint8_t PlayChoice : 1;
		uint8_t VSUniSystem : 1;
	} Flags7;
	struct {
		uint8_t PRGRAMSize;
	} Flags8;
	struct {
		uint8_t Reserved : 7;
		uint8_t TVSystem : 1; // 0: NTSC; 1: PAL
	} Flags9;
	struct {
		uint8_t Unknown : 2;
		uint8_t HasBusConflicts : 1;
		uint8_t HasPRGRAM : 1;
		uint8_t Unknown2 : 2;
		uint8_t TVSystem : 2; // 0: NTSC; 2: PAL; 1/3: dual compatible
	} Flags10;
	uint8_t unused[4];
};
static_assert(sizeof(NESFileHeader) == 16);

struct NESFileData {
	std::vector<uint8_t> PRGROM;
	std::vector<uint8_t> CHRROM;
};

struct NESFile {
	explicit NESFile(const std::filesystem::path &romPath);

	bool isValid();

	NESFileHeader Header;
	NESFileData Data;
};