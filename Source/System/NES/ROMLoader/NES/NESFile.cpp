#include <fstream>
#include "NESFile.hpp"
#include "Utils/Logger.hpp"

NESFile::NESFile(const std::filesystem::path &romPath) {
	std::vector<char> fileData;
	{
		std::ifstream romRead(romPath, std::ifstream::ate | std::ifstream::binary);

		std::ifstream::pos_type size = romRead.tellg();
		fileData.resize(size);

		romRead.seekg(0, std::ifstream::beg);
		romRead.read(fileData.data(), size);
	}

	const char *curFilePos = fileData.data();
	Header = *(NESFileHeader*) curFilePos;

	if (!isValid()) {
		ELOG("ROM file is not a NES File.");
		return;
	}

	Header.PRGSize = Header.PRGSize * 1024 * 16;
	Header.CHRSize = Header.CHRSize * 1024 * 8;

	Data.PRGROM = Data.PRGROM;
	Data.PRGROM.resize(Header.PRGSize);
	Data.CHRROM.resize(Header.CHRSize);

	curFilePos += sizeof(NESFileHeader);
	std::copy(curFilePos,
			  curFilePos + Header.PRGSize,
			  Data.PRGROM.data());

	curFilePos += Header.PRGSize;
	std::copy(curFilePos,
			  curFilePos + Header.CHRSize,
			  Data.CHRROM.data());
	DLOG("ROM file successfully read");
}

bool NESFile::isValid() {
	return Header.MagicNumber == 0x1A53454E;
}
