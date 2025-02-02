use crate::system::nes::apu::APU;
use crate::system::nes::cpu::CPU;
use crate::system::nes::memory::Memory;
use crate::system::nes::ppu::PPU;

pub struct IOBus {
    pub cpu: CPU,
    pub memory: Memory,
    pub ppu: PPU,
    pub apu: APU,
}

impl IOBus {
    pub fn new() -> Self {
        Self {
            cpu: CPU::new(),
            memory: Memory::new(),
            ppu: PPU::new(),
            apu: APU::new(),
        }
    }
}