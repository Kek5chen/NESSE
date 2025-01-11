use bitflags::bitflags;
use crate::system::nes::mnemonics::MNEMONICS;

bitflags! {
    pub struct CPUFlagStruct : u8 {
        const Negative = 0b0000_0001;
        const Overflow = 0b0000_0010;
        const Reserved = 0b0000_0100;
        const Break = 0b0000_1000;
        const Decimal = 0b0001_0000;
        const InterruptDisable = 0b0010_0000;
        const Zero = 0b0100_0000;
        const Carry = 0b1000_0000;
    }
}

pub struct CPU {
    pub a: u8,
    pub x: u8,
    pub y: u8,
    pub p: u8,
    pub pc: u16,
    pub sp: u8,
    pub flags: CPUFlagStruct,
}

impl CPU {
    pub fn new() -> Self {
        Self {
            a: 0,
            x: 0,
            y: 0,
            p: 0,
            pc: 0xFFFC,
            sp: 0xFF,
            flags: CPUFlagStruct::empty(),
        }
    }

    /// start_address is read from memory at 0xFFFC
    pub fn reset(&mut self, start_address: u16) {
        self.sp = 0xFF;
        self.flags = CPUFlagStruct::empty() | CPUFlagStruct::InterruptDisable;
        self.pc = start_address;
    }

    pub fn negative(&self) -> bool {
        self.flags.contains(CPUFlagStruct::Negative)
    }
    pub fn overflow(&self) -> bool {
        self.flags.contains(CPUFlagStruct::Overflow)
    }
    pub fn reserved(&self) -> bool {
        self.flags.contains(CPUFlagStruct::Reserved)
    }
    pub fn break_(&self) -> bool {
        self.flags.contains(CPUFlagStruct::Break)
    }

    pub fn decimal(&self) -> bool {
        self.flags.contains(CPUFlagStruct::Decimal)
    }
    pub fn interrupt_disable(&self) -> bool {
        self.flags.contains(CPUFlagStruct::InterruptDisable)
    }
    pub fn zero(&self) -> bool {
        self.flags.contains(CPUFlagStruct::Zero)
    }

    pub fn carry(&self) -> bool {
        self.flags.contains(CPUFlagStruct::Carry)
    }

    pub fn set_negative(&mut self, value: bool) {
        if value {
            self.flags.insert(CPUFlagStruct::Negative);
        } else {
            self.flags.remove(CPUFlagStruct::Negative);
        }
    }
    pub fn set_overflow(&mut self, value: bool) {
        if value {
            self.flags.insert(CPUFlagStruct::Overflow);
        }
    }

    pub fn set_break(&mut self, value: bool) {
        if value {
            self.flags.insert(CPUFlagStruct::Break);
        } else {
            self.flags.remove(CPUFlagStruct::Break);
        }
    }

    pub fn set_decimal(&mut self, value: bool) {
        if value {
            self.flags.insert(CPUFlagStruct::Decimal);
        }
    }

    pub fn set_interrupt_disable(&mut self, value: bool) {
        if value {
            self.flags.insert(CPUFlagStruct::InterruptDisable);
        } else {
            self.flags.remove(CPUFlagStruct::InterruptDisable);
        }
    }

    pub fn set_zero(&mut self, value: bool) {
        if value {
            self.flags.insert(CPUFlagStruct::Zero);
        }
    }

    pub fn set_carry(&mut self, value: bool) {
        if value {
            self.flags.insert(CPUFlagStruct::Carry);
        }
    }
}

pub const fn get_mnemonic(opcode: u8) -> &'static str {
    MNEMONICS[opcode as usize].0
}

pub const fn get_opcode_size(opcode: u8) -> u8 {
    MNEMONICS[opcode as usize].1
}