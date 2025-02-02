use std::path::Path;
use crate::system::nes::file::NESFile;
use crate::system::nes::NES;

pub struct NESLoader;

impl NESLoader {
    pub fn load_rom<P: AsRef<Path>>(path: &P, nes: &mut NES) -> anyhow::Result<()>{
        let rom = NESFile::new(path)?;
        if !rom.is_valid() {
            return Err(anyhow::anyhow!("Invalid NES ROM"));
        }

        nes.bus.memory.write_slice(0xC000, &rom.data.prg_rom)?;
        nes.bus.ppu.write_slice(0, &rom.data.chr_rom)?;

        Ok(())
    }
}