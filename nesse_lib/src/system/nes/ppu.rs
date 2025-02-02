use std::mem::size_of;
use anyhow::format_err;
use num_traits::ToBytes;

pub struct PPU {
    memory: [u8; 0x3FFF],
}

impl PPU {
    pub fn new() -> Self {
        Self {
            memory: [0; 0x3FFF],
        }
    }

    pub fn write<T: ToBytes>(&mut self, dst: u16, data: &T) -> anyhow::Result<()> {
        let size = size_of::<T>();
        if dst as usize + size > self.memory.len() {
            return Err(format_err!("Out of Bounds Write to 0x{dst:x?} of size {size}"));
        }

        let binding = data.to_be_bytes();
        let bytes = binding.as_ref();
        self.memory[dst as usize..dst as usize + size].copy_from_slice(&bytes);

        Ok(())
    }
    
    pub fn write_slice(&mut self, dst: u16, data: &[u8]) -> anyhow::Result<()> {
        if dst as usize + data.len() > self.memory.len() {
            return Err(format_err!("Out of Bounds Write to 0x{dst:x?} of size {}", data.len()));
        }

        self.memory[dst as usize..dst as usize + data.len()].copy_from_slice(data);

        Ok(())
    }
}