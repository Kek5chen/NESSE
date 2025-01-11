use std::fs::File;
use std::io::Read;
use std::path::Path;
use static_assertions::const_assert_eq;

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct NESFileHeader {
    pub magic_number: u32, // 4 bytes
    pub prg_size: u8,
    pub chr_size: u8,
    pub flags6: u8,
    pub flags7: u8,
    pub flags8: u8,
    pub flags9: u8,
    pub flags10: u8,
    pub unused: [u8; 4], // 4 bytes
}

// Ensures the struct is 16 bytes in total.
const_assert_eq!(std::mem::size_of::<NESFileHeader>(), 16);

#[derive(Debug)]
pub struct NESFileData {
    pub prg_rom: Vec<u8>,
    pub chr_rom: Vec<u8>,
}

#[derive(Debug)]
pub struct NESFile {
    pub header: NESFileHeader,
    pub data: NESFileData,
}

impl NESFile {
    pub fn new<P: AsRef<Path>>(rom_path: &P) -> std::io::Result<Self> {
        let mut file = File::open(rom_path)?;

        // Read the 16-byte header
        let mut raw_header = [0u8; 16];
        file.read_exact(&mut raw_header)?;

        // Convert the raw bytes into our NESFileHeader.
        // SAFETY: We know raw_header is exactly 16 bytes, matching the struct’s size.
        let header: NESFileHeader = unsafe { std::mem::transmute(raw_header) };

        // Prepare to read PRG/CHR data based on header
        let prg_len = (header.prg_size as usize) * 16384; // PRG is in 16 KB units
        let chr_len = (header.chr_size as usize) * 8192;  // CHR is in 8 KB units

        // Read PRG ROM
        let mut prg_rom = vec![0u8; prg_len];
        file.read_exact(&mut prg_rom)?;

        // Read CHR ROM
        let mut chr_rom = vec![0u8; chr_len];
        file.read_exact(&mut chr_rom)?;

        // TODO: Trainers

        Ok(Self {
            header,
            data: NESFileData { prg_rom, chr_rom },
        })
    }


    pub fn is_valid(&self) -> bool {
        self.header.magic_number == 0x1A53454E
    }

    pub fn mapper_lower(&self) -> u8 {
        self.header.flags6 & 0x0F
    }
}