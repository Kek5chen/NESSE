use std::mem::size_of;
use anyhow::format_err;
use num_traits::{FromBytes, ToBytes};

pub struct Memory {
    ram: [u8; 0x800],             // 0x0000 - 0x07FF mirrored to 0x1FFF - 0x1FFF
    ppu: [u8; 0x8],               // 0x2000 - 0x2007
    apu_io_registers: [u8; 0x18], // 0x4000 - 0x4017
    cartridge_map: [u8; 0xBFE0],  // 0x4020 - 0xFFFF
}

impl Memory {
    pub fn new() -> Self {
        Self {
            ram: [0; 0x800],
            ppu: [0; 0x8],
            apu_io_registers: [0; 0x18],
            cartridge_map: [0; 0xBFE0],
        }
    }

    pub fn write<T: ToBytes>(&mut self, mut dst: u16, value: T) -> anyhow::Result<()> {
        let size = size_of::<T>();
        if dst as usize + size > 0x10000 {
            return Err(format_err!("Out of Bounds Write to 0x{dst:x?} of size {size}"));
        }
        let binding = value.to_be_bytes();
        let byte_data = binding.as_ref();

        for byte in byte_data {
            if dst <= 0x07FF {
                self.ram[dst as usize] = *byte;
            } else if dst >= 0x0800 && dst <= 0x1FFF {
                self.ram[dst as usize - 0x0800] = *byte;
            } else if dst >= 0x2000 && dst <= 0x2007 {
                self.ppu[dst as usize - 0x2000] = *byte;
            } else if dst >= 0x4000 && dst <= 0x4017 {
                self.apu_io_registers[dst as usize - 0x4000] = *byte;
            } else if dst >= 0x4020 {
                self.cartridge_map[dst as usize - 0x4020] = *byte;
            }

            dst += 1;
        }

        Ok(())
    }
    
    pub fn write_slice(&mut self, dst: u16, data: &[u8]) -> anyhow::Result<()> {
        if dst as usize + data.len() > 0x10000 {
            return Err(format_err!("Out of Bounds Write to 0x{dst:x?} of size {}", data.len()));
        }

        for (i, byte) in data.iter().enumerate() {
            let cur_dst = dst + i as u16;
            if cur_dst <= 0x07FF {
                self.ram[cur_dst as usize] = *byte;
            } else if cur_dst >= 0x0800 && cur_dst <= 0x1FFF {
                self.ram[cur_dst as usize - 0x0800] = *byte;
            } else if cur_dst >= 0x2000 && cur_dst <= 0x2007 {
                self.ppu[cur_dst as usize - 0x2000] = *byte;
            } else if cur_dst >= 0x4000 && cur_dst <= 0x4017 {
                self.apu_io_registers[cur_dst as usize - 0x4000] = *byte;
            } else if cur_dst >= 0x4020 {
                self.cartridge_map[cur_dst as usize - 0x4020] = *byte;
            } else {
                return Err(format_err!("Invalid Memory Address 0x{cur_dst:x?}"));
            }
        }

        Ok(())
    }

    pub fn read<T: FromBytes + Copy + bytemuck::Pod>(&self, addr: u16) -> anyhow::Result<T> {
        let size = size_of::<T>();
        if addr as usize + size > 0x10000 {
            return Err(format_err!("Out of Bounds Read at 0x{addr:x?}"));
        }

        let bytes;

        if addr <= 0x07FF {
            bytes = &self.ram[addr as usize..addr as usize + size];
        } else if addr >= 0x0800 && addr <= 0x1FFF {
            bytes = &self.ram[addr as usize - 0x0800.. addr as usize - 0x0800 + size];
        } else if addr >= 0x2000 && addr <= 0x2007 {
            bytes = &self.ppu[addr as usize - 0x2000..addr as usize - 0x2000 + size];
        } else if addr >= 0x4000 && addr <= 0x4017 {
            bytes = &self.apu_io_registers[addr as usize - 0x4000..addr as usize - 0x4000 + size];
        } else if addr >= 0x4020 {
            bytes = &self.cartridge_map[addr as usize - 0x4020..addr as usize - 0x4020 + size];
        } else {
            return Err(format_err!("Invalid Memory Address 0x{addr:x?}"));
        }

        Ok(*bytemuck::from_bytes(bytes))
    }
}