use std::path::Path;
use std::fmt::Debug;
use crate::system::nes::cpu::get_opcode_size;
use crate::system::nes::iobus::IOBus;
use crate::system::nes::loader::NESLoader;
use crate::system::nes::opcodes::OPCODES;

pub mod iobus;
pub mod cpu;
pub mod memory;
pub mod ppu;
pub mod apu;
pub mod opcodes;
pub mod mnemonics;
pub mod loader;
mod file;

const CPU_TICK_COUNT: u32 = 1_789_773;
const CYCLES_PER_FRAME_NTSC: u16 = (CPU_TICK_COUNT / 60) as u16;
pub struct NES {
    cycles_left: u16,
    cycles_per_frame: u16,
    pub bus: IOBus,
}

impl NES {
    pub fn new() -> Self {
        Self {
            cycles_left: 0,
            cycles_per_frame: CYCLES_PER_FRAME_NTSC,
            bus: IOBus::new()
        }
    }

    pub fn insert_rom<P: AsRef<Path> + Debug>(&mut self, path: &P) -> anyhow::Result<()> {
        println!("Loading {path:?}...");

        NESLoader::load_rom(path, self)?;

        Ok(())
    }

    pub fn reset(&mut self) -> anyhow::Result<()> {
        let start_addr = self.bus.memory.read(0xFFFC)?;
        self.bus.cpu.reset(start_addr);
        Ok(())
    }

    pub fn run(&mut self) -> anyhow::Result<()> {
        println!("Starting Nes...");
        self.reset()?;
        for _ in 0..100 {
            self.execute()?;
        }
        // TODO: self.next_frame(); - run next_frame once it's safe

        Ok(())
    }

    pub fn get_instruction_at(&self, addr: u16) -> anyhow::Result<Vec<u8>> {
        let mut instruction = vec![];
        println!("Getting instruction at 0x{:04X}", addr);
        let opcode = self.bus.memory.read(addr)?;
        instruction.push(opcode);

        let instruction_size = get_opcode_size(opcode);
        for i in 1..=instruction_size {
            instruction.push(self.bus.memory.read(addr + i as u16)?);
        }

        Ok(instruction)
    }

    pub fn get_instruction(&self) -> anyhow::Result<Vec<u8>> {
        self.get_instruction_at(self.bus.cpu.pc)
    }

    pub fn execute(&mut self) -> anyhow::Result<u8> {
        let cur_instruction = self.get_instruction()?;

        let opcode = cur_instruction[0];
        let byte1 = *cur_instruction.get(1).unwrap_or(&0);
        let byte2 = *cur_instruction.get(2).unwrap_or(&0);

        OPCODES[opcode as usize].0(self, byte1, byte2)?;

        self.bus.cpu.pc += 1 + cur_instruction.len() as u16;

        Ok(0)
    }

    pub fn next_frame(&mut self) -> anyhow::Result<()> {
        self.cycles_left = self.cycles_per_frame;
        while self.cycles_left > 0 {
            self.cycles_left -= self.execute()? as u16;
        }

        Ok(())
    }
}