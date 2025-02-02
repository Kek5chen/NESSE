use crate::system::nes::cpu::CPUFlagStruct;
use crate::system::nes::NES;

macro_rules! update_register {
    ($nes:ident, $field:ident, $value:expr) => {
        $nes.bus.cpu.$field = $value;
        $nes.bus.cpu.set_zero($value == 0);
        $nes.bus.cpu.set_negative($value == 0);
    }
}

// 0x00
pub fn brk_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    let pc_high = (nes.bus.cpu.pc >> 8) as u8;
    let pc_low  = (nes.bus.cpu.pc & 0xFF) as u8;

    nes.bus.memory.write(nes.bus.cpu.sp as u16, pc_high)?;
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_sub(1);
    nes.bus.memory.write(nes.bus.cpu.sp as u16, pc_low)?;
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_sub(1);

    nes.bus.memory.write(nes.bus.cpu.sp as u16, nes.bus.cpu.p)?;
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_sub(1);

    nes.bus.cpu.flags &= CPUFlagStruct::Break;
    nes.bus.cpu.flags &= CPUFlagStruct::InterruptDisable;

    // Read new PC from 0xFFFE (little-endian)
    let new_pc = nes.bus.memory.read(0xFFFE)?;
    nes.bus.cpu.pc = new_pc;
    Ok(())
}

pub fn lda_immediate(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    update_register!(nes, a, byte1);
    Ok(())
}

pub fn sta_absolute(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let addr = (byte2 as u16) << 8 | (byte1 as u16);
    nes.bus.memory.write(addr, nes.bus.cpu.a)?;
    Ok(())
}

pub fn lda_absolute(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let addr = (byte2 as u16) << 8 | (byte1 as u16);
    let value = nes.bus.memory.read(addr)?;
    update_register!(nes, a, value);
    Ok(())
}

pub fn bpl_relative(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    if !nes.bus.cpu.negative() {
        // Sign-extend `byte1` by casting to i8 and then add
        nes.bus.cpu.pc = nes.bus.cpu.pc.wrapping_add(byte1 as i8 as u16);
    }
    Ok(())
}

pub fn bmi_relative(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    if nes.bus.cpu.negative() {
        nes.bus.cpu.pc = nes.bus.cpu.pc.wrapping_add(byte1 as i8 as u16);
    }
    Ok(())
}

pub fn sei_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    nes.bus.cpu.set_interrupt_disable(true);
    Ok(())
}

pub fn cld_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    nes.bus.cpu.set_decimal(false);
    Ok(())
}

pub fn ldx_immediate(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    update_register!(nes, x, byte1);
    Ok(())
}

pub fn txs_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    update_register!(nes, sp, nes.bus.cpu.x);
    Ok(())
}

pub fn txa_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    update_register!(nes, a, nes.bus.cpu.x);
    Ok(())
}

pub fn inx_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    let new_val = nes.bus.cpu.x.wrapping_add(1);
    update_register!(nes, x, new_val);
    Ok(())
}

pub fn dex_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    let new_val = nes.bus.cpu.x.wrapping_sub(1);
    update_register!(nes, x, new_val);
    Ok(())
}

pub fn bne_relative(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    if !nes.bus.cpu.zero() {
        nes.bus.cpu.pc = nes.bus.cpu.pc.wrapping_add(byte1 as i8 as u16);
    }
    Ok(())
}

pub fn lda_absolute_ix(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let addr = (byte2 as u16) << 8 | (byte1 as u16);
    let value = nes.bus.memory.read::<u8>(addr)?.wrapping_add(nes.bus.cpu.x);
    update_register!(nes, a, value);
    Ok(())
}

pub fn cmp_absolute_ix(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let addr = (byte2 as u16) << 8 | (byte1 as u16);
    let value = nes.bus.memory.read::<u8>(addr)?.wrapping_add(nes.bus.cpu.x);

    let other_value = nes.bus.cpu.a;
    let subtraction = (value as i16) - (other_value as i16);

    nes.bus.cpu.set_carry(value >= other_value);
    nes.bus.cpu.set_zero(subtraction == 0);
    nes.bus.cpu.set_negative((subtraction as i8) < 0);
    Ok(())
}

pub fn sta_absolute_ix(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let addr = (byte2 as u16) << 8 | (byte1 as u16);
    nes.bus.memory.write(addr.wrapping_add(nes.bus.cpu.x as u16), nes.bus.cpu.a)?;
    Ok(())
}

pub fn jsr_absolute(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let addr = (byte2 as u16) << 8 | (byte1 as u16);

    // For a typical 6502 JSR, we push the address-1 of the last byte of the JSR instruction.
    // But your C++ code had something like: `uint16_t returnAddr = ioBus->mCPU.PC - 2;`
    // We'll replicate that directly.
    let return_addr = nes.bus.cpu.pc.wrapping_sub(2);

    let hi = (return_addr >> 8) as u8;
    let lo = (return_addr & 0xFF) as u8;

    nes.bus.memory.write(nes.bus.cpu.sp as u16, hi)?;
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_sub(1);
    nes.bus.memory.write(nes.bus.cpu.sp as u16, lo)?;
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_sub(1);

    nes.bus.cpu.pc = addr;
    Ok(())
}

pub fn nop_immediate(_nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    Ok(())
}

pub fn ora_indirect_y(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    let addr_base = nes.bus.memory.read::<u8>(byte1 as u16)? as u16;
    let addr = addr_base.wrapping_add(nes.bus.cpu.y as u16);
    let value = nes.bus.memory.read::<u8>(addr)?;

    let result = nes.bus.cpu.a | value;
    nes.bus.cpu.a = result;

    nes.bus.cpu.set_zero(result == 0);
    nes.bus.cpu.set_negative((result as i8) < 0);
    Ok(())
}

/// ORA (Indirect,X) - opcode 0x01
///   - Reads from `(byte1 + X) & 0xFF` in zero page to get a 16-bit address,
///     then ORs that value with A.
pub fn ora_indirect_x(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    // Zero-page address = (operand + X) & 0xFF
    let zp_addr = (byte1.wrapping_add(nes.bus.cpu.x)) as u16;

    // Low and high bytes of the target address
    let low: u16 = nes.bus.memory.read(zp_addr)?;
    let high: u16 = nes.bus.memory.read((zp_addr.wrapping_add(1)) & 0xFF)?;

    let final_addr = ((high) << 8) | (low);
    let value: u8 = nes.bus.memory.read(final_addr)?;

    let result = nes.bus.cpu.a | value;
    nes.bus.cpu.a = result;

    nes.bus.cpu.set_zero(result == 0);
    nes.bus.cpu.set_negative((result as i8) < 0);
    Ok(())
}

/// ORA Zero Page - opcode 0x05
pub fn ora_zero_page(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    let addr = byte1 as u16;
    let value: u8 = nes.bus.memory.read(addr)?;
    let result = nes.bus.cpu.a | value;
    nes.bus.cpu.a = result;

    nes.bus.cpu.set_zero(result == 0);
    nes.bus.cpu.set_negative((result as i8) < 0);
    Ok(())
}

/// ORA Absolute - opcode 0x0D
pub fn ora_absolute(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let addr = ((byte2 as u16) << 8) | (byte1 as u16);
    let value: u8 = nes.bus.memory.read(addr)?;
    let result = nes.bus.cpu.a | value;
    nes.bus.cpu.a = result;

    nes.bus.cpu.set_zero(result == 0);
    nes.bus.cpu.set_negative((result as i8) < 0);
    Ok(())
}

/// ORA Zero Page,X - opcode 0x15
pub fn ora_zp_x(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    let addr = (byte1.wrapping_add(nes.bus.cpu.x)) as u16;
    let value: u8 = nes.bus.memory.read(addr)?;
    let result = nes.bus.cpu.a | value;
    nes.bus.cpu.a = result;

    nes.bus.cpu.set_zero(result == 0);
    nes.bus.cpu.set_negative((result as i8) < 0);
    Ok(())
}

/// ORA Absolute,Y - opcode 0x19
pub fn ora_absolute_y(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let base_addr = ((byte2 as u16) << 8) | (byte1 as u16);
    let addr = base_addr.wrapping_add(nes.bus.cpu.y as u16);
    let value: u8 = nes.bus.memory.read(addr)?;
    let result = nes.bus.cpu.a | value;
    nes.bus.cpu.a = result;

    nes.bus.cpu.set_zero(result == 0);
    nes.bus.cpu.set_negative((result as i8) < 0);
    Ok(())
}

/// ORA Absolute,X - opcode 0x1D
pub fn ora_absolute_x(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let base_addr = ((byte2 as u16) << 8) | (byte1 as u16);
    let addr = base_addr.wrapping_add(nes.bus.cpu.x as u16);
    let value: u8 = nes.bus.memory.read(addr)?;
    let result = nes.bus.cpu.a | value;
    nes.bus.cpu.a = result;

    nes.bus.cpu.set_zero(result == 0);
    nes.bus.cpu.set_negative((result as i8) < 0);
    Ok(())
}

// ----------------------------------------------------------

/// ASL Zero Page - opcode 0x06
pub fn asl_zero_page(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    let addr = byte1 as u16;
    let value: u8 = nes.bus.memory.read(addr)?;
    let carry = (value & 0x80) != 0;

    let shifted = value << 1;
    nes.bus.memory.write(addr, shifted)?;

    nes.bus.cpu.set_carry(carry);
    nes.bus.cpu.set_zero(shifted == 0);
    nes.bus.cpu.set_negative((shifted as i8) < 0);
    Ok(())
}

/// ASL Accumulator - opcode 0x0A
pub fn asl_accumulator(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    let carry = (nes.bus.cpu.a & 0x80) != 0;
    nes.bus.cpu.a <<= 1;

    nes.bus.cpu.set_carry(carry);
    nes.bus.cpu.set_zero(nes.bus.cpu.a == 0);
    nes.bus.cpu.set_negative((nes.bus.cpu.a as i8) < 0);
    Ok(())
}

/// ASL Absolute - opcode 0x0E
pub fn asl_absolute(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let addr = ((byte2 as u16) << 8) | (byte1 as u16);
    let value: u8 = nes.bus.memory.read(addr)?;
    let carry = (value & 0x80) != 0;

    let shifted = value << 1;
    nes.bus.memory.write(addr, shifted)?;

    nes.bus.cpu.set_carry(carry);
    nes.bus.cpu.set_zero(shifted == 0);
    nes.bus.cpu.set_negative((shifted as i8) < 0);
    Ok(())
}

/// ASL Absolute,X - opcode 0x1E
pub fn asl_absolute_x(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let base = ((byte2 as u16) << 8) | (byte1 as u16);
    let addr = base.wrapping_add(nes.bus.cpu.x as u16);

    let value: u8 = nes.bus.memory.read(addr)?;
    let carry = (value & 0x80) != 0;

    let shifted = value << 1;
    nes.bus.memory.write(addr, shifted)?;

    nes.bus.cpu.set_carry(carry);
    nes.bus.cpu.set_zero(shifted == 0);
    nes.bus.cpu.set_negative((shifted as i8) < 0);
    Ok(())
}

// ----------------------------------------------------------

/// PHP (Push Processor Status) - opcode 0x08
pub fn php_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    let p = nes.bus.cpu.p; // Processor status
    nes.bus.memory.write(nes.bus.cpu.sp as u16, p)?;
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_sub(1);
    Ok(())
}

/// PLP (Pull Processor Status) - opcode 0x28
pub fn plp_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_add(1);
    let new_p = nes.bus.memory.read(nes.bus.cpu.sp as u16)?;
    nes.bus.cpu.p = new_p;
    Ok(())
}

/// PHA (Push A) - opcode 0x48
pub fn pha_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    let a = nes.bus.cpu.a;
    nes.bus.memory.write(nes.bus.cpu.sp as u16, a)?;
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_sub(1);
    Ok(())
}

/// PLA (Pull A) - opcode 0x68
pub fn pla_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_add(1);
    let value = nes.bus.memory.read(nes.bus.cpu.sp as u16)?;
    update_register!(nes, a, value);
    Ok(())
}

// ----------------------------------------------------------

/// CLC (Clear Carry) - opcode 0x18
pub fn clc_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    nes.bus.cpu.set_carry(false);
    Ok(())
}

/// SEC (Set Carry) - opcode 0x38
pub fn sec_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    nes.bus.cpu.set_carry(true);
    Ok(())
}

// ----------------------------------------------------------

/// BIT Zero Page - opcode 0x24
pub fn bit_zero_page(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    let addr = byte1 as u16;
    let value: u8 = nes.bus.memory.read(addr)?;
    let result = nes.bus.cpu.a & value;

    nes.bus.cpu.set_zero(result == 0);
    // For BIT, bits 7 and 6 of `value` get copied into N and V flags.
    nes.bus.cpu.set_negative((value & 0x80) != 0);
    nes.bus.cpu.set_overflow((value & 0x40) != 0);
    Ok(())
}

/// BIT Absolute - opcode 0x2C
pub fn bit_absolute(nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    let addr = ((byte2 as u16) << 8) | (byte1 as u16);
    let value: u8 = nes.bus.memory.read(addr)?;
    let result = nes.bus.cpu.a & value;

    nes.bus.cpu.set_zero(result == 0);
    nes.bus.cpu.set_negative((value & 0x80) != 0);
    nes.bus.cpu.set_overflow((value & 0x40) != 0);
    Ok(())
}

// ----------------------------------------------------------

/// RTI - opcode 0x40
/// Similar to PLP, but also pulls PC from stack.
pub fn rti_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    // Pull status
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_add(1);
    let new_p = nes.bus.memory.read(nes.bus.cpu.sp as u16)?;
    nes.bus.cpu.p = new_p;

    // Pull low PC
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_add(1);
    let pcl: u16 = nes.bus.memory.read::<u8>(nes.bus.cpu.sp as u16)? as u16;

    // Pull high PC
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_add(1);
    let pch: u16 = nes.bus.memory.read::<u8>(nes.bus.cpu.sp as u16)? as u16;

    nes.bus.cpu.pc = (pch << 8) | pcl;
    Ok(())
}

/// RTS - opcode 0x60
/// Pull PC from stack, then PC++
pub fn rts_implied(nes: &mut NES, _byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    // Pull low PC
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_add(1);
    let pcl = nes.bus.memory.read::<u8>(nes.bus.cpu.sp as u16)? as u16;

    // Pull high PC
    nes.bus.cpu.sp = nes.bus.cpu.sp.wrapping_add(1);
    let pch = nes.bus.memory.read::<u8>(nes.bus.cpu.sp as u16)? as u16;

    nes.bus.cpu.pc = ((pch << 8) | pcl).wrapping_add(1);
    Ok(())
}

// 0xF0
pub fn beq_relative(nes: &mut NES, byte1: u8, _byte2: u8) -> anyhow::Result<()> {
    if nes.bus.cpu.flags.contains(CPUFlagStruct::Zero) {
        nes.bus.cpu.pc = nes.bus.cpu.pc + byte1 as u16;
    }

    Ok(())
}

pub fn ins_nullfunc(_nes: &mut NES, byte1: u8, byte2: u8) -> anyhow::Result<()> {
    panic!(
        "INVALID! Game called NULLFUNC with 0x{:02X} and 0x{:02X}",
        byte1, byte2
    );
}

// function and cycle amount
pub const OPCODES: [(fn(&mut NES, u8, u8) -> anyhow::Result<()>, u8); 256] = [
///////////// 00 /////////////
    (brk_implied, 7),		// 0x00
    (ora_indirect_x, 6),	// 0x01;
    (ins_nullfunc, 0),		// 0x02;
    (ins_nullfunc, 0),		// 0x03;
    (nop_immediate, 3),		// 0x04;
    (ora_zero_page, 3),		// 0x05;
    (asl_zero_page, 5),		// 0x06;
    (ins_nullfunc, 0),		// 0x07
    (php_implied, 3),		// 0x08
    (ins_nullfunc, 0),		// 0x09
    (asl_accumulator, 2),   // 0x0A
    (ins_nullfunc, 0),		// 0x0B
    (nop_immediate, 4),		// 0x0C
    (ora_absolute, 2),		// 0x0D
    (asl_absolute, 4),	    // 0x0E
    (ins_nullfunc, 0),		// 0x0F
////////////// 10 /////////////
    (bpl_relative, 1),		// 0x10
    (ora_indirect_y, 5),	// 0x11
    (ins_nullfunc, 0),		// 0x12
    (ins_nullfunc, 0),		// 0x13
    (nop_immediate, 4),		// 0x14
    (ora_zp_x, 4),		    // 0x15
    (ins_nullfunc, 0),		// 0x16
    (ins_nullfunc, 0),		// 0x17
    (clc_implied, 2),		// 0x18
    (ora_absolute_y, 4),	// 0x19
    (nop_immediate, 2),		// 0x1A
    (ins_nullfunc, 0),		// 0x1B
    (nop_immediate, 4),		// 0x1C
    (ins_nullfunc, 0),		// 0x1D
    (ins_nullfunc, 0),		// 0x1E
    (ins_nullfunc, 0),		// 0x1F
////////////// 20 /////////////
    (jsr_absolute, 1),		// 0x20
    (ins_nullfunc, 0),		// 0x21
    (ins_nullfunc, 0),		// 0x22
    (ins_nullfunc, 0),		// 0x23
    (bit_zero_page, 3),		// 0x24
    (ins_nullfunc, 0),		// 0x25
    (ins_nullfunc, 0),		// 0x26
    (ins_nullfunc, 0),		// 0x27
    (plp_implied, 4),		// 0x28
    (ins_nullfunc, 0),		// 0x29
    (ins_nullfunc, 0),		// 0x2A
    (ins_nullfunc, 0),		// 0x2B
    (bit_absolute, 4),		// 0x2C
    (ins_nullfunc, 0),		// 0x2D
    (ins_nullfunc, 0),		// 0x2E
    (ins_nullfunc, 0),		// 0x2F
////////////// 30 /////////////
    (bmi_relative, 1),		// 0x30
    (ins_nullfunc, 0),		// 0x31
    (ins_nullfunc, 0),		// 0x32
    (ins_nullfunc, 0),		// 0x33
    (nop_immediate, 4),		// 0x34
    (ins_nullfunc, 0),		// 0x35
    (ins_nullfunc, 0),		// 0x36
    (ins_nullfunc, 0),		// 0x37
    (sec_implied, 2),		// 0x38
    (ins_nullfunc, 0),		// 0x39
    (nop_immediate, 2),		// 0x3A
    (ins_nullfunc, 0),		// 0x3B
    (nop_immediate, 4),		// 0x3C
    (ins_nullfunc, 0),		// 0x3D
    (ins_nullfunc, 0),		// 0x3E
    (ins_nullfunc, 0),		// 0x3F
////////////// 40 /////////////
    (rti_implied, 6),		// 0x40
    (ins_nullfunc, 0),		// 0x41
    (ins_nullfunc, 0),		// 0x42
    (ins_nullfunc, 0),		// 0x43
    (nop_immediate, 3),		// 0x44
    (ins_nullfunc, 0),		// 0x45
    (ins_nullfunc, 0),		// 0x46
    (ins_nullfunc, 0),		// 0x47
    (pha_implied, 3),		// 0x48
    (ins_nullfunc, 0),		// 0x49
    (ins_nullfunc, 0),		// 0x4A
    (ins_nullfunc, 0),		// 0x4B
    (ins_nullfunc, 0),		// 0x4C
    (ins_nullfunc, 0),		// 0x4D
    (ins_nullfunc, 0),		// 0x4E
    (ins_nullfunc, 0),		// 0x4F
////////////// 50 /////////////
    (ins_nullfunc, 1),		// 0x50
    (ins_nullfunc, 0),		// 0x51
    (ins_nullfunc, 0),		// 0x52
    (ins_nullfunc, 0),		// 0x53
    (nop_immediate, 4),		// 0x54
    (ins_nullfunc, 0),		// 0x55
    (ins_nullfunc, 0),		// 0x56
    (ins_nullfunc, 0),		// 0x57
    (ins_nullfunc, 0),		// 0x58
    (ins_nullfunc, 0),		// 0x59
    (nop_immediate, 2),		// 0x5A
    (ins_nullfunc, 0),		// 0x5B
    (nop_immediate, 4),		// 0x5C
    (ins_nullfunc, 0),		// 0x5D
    (ins_nullfunc, 0),		// 0x5E
    (ins_nullfunc, 0),		// 0x5F
////////////// 60 /////////////
    (rts_implied, 6),		// 0x60
    (ins_nullfunc, 0),		// 0x61
    (ins_nullfunc, 0),		// 0x62
    (ins_nullfunc, 0),		// 0x63
    (nop_immediate, 3),		// 0x64
    (ins_nullfunc, 0),		// 0x65
    (ins_nullfunc, 0),		// 0x66
    (ins_nullfunc, 0),		// 0x67
    (pla_implied, 4),		// 0x68
    (ins_nullfunc, 0),		// 0x69
    (ins_nullfunc, 0),		// 0x6A
    (ins_nullfunc, 0),		// 0x6B
    (ins_nullfunc, 0),		// 0x6C
    (ins_nullfunc, 0),		// 0x6D
    (ins_nullfunc, 0),		// 0x6E
    (ins_nullfunc, 0),		// 0x6F
////////////// 70 /////////////
    (ins_nullfunc, 1),		// 0x70
    (ins_nullfunc, 0),		// 0x71
    (ins_nullfunc, 0),		// 0x72
    (ins_nullfunc, 0),		// 0x73
    (nop_immediate, 4),		// 0x74
    (ins_nullfunc, 0),		// 0x75
    (ins_nullfunc, 0),		// 0x76
    (ins_nullfunc, 0),		// 0x77
    (sei_implied, 1),		// 0x78
    (ins_nullfunc, 0),		// 0x79
    (nop_immediate, 2),		// 0x7A
    (ins_nullfunc, 0),		// 0x7B
    (nop_immediate, 4),		// 0x7C
    (ins_nullfunc, 0),		// 0x7D
    (ins_nullfunc, 0),		// 0x7E
    (ins_nullfunc, 0),		// 0x7F
////////////// 80 /////////////
    (nop_immediate, 2),		// 0x80
    (ins_nullfunc, 0),		// 0x81
    (nop_immediate, 2),		// 0x82
    (ins_nullfunc, 0),		// 0x83
    (ins_nullfunc, 0),		// 0x84
    (ins_nullfunc, 0),		// 0x85
    (ins_nullfunc, 0),		// 0x86
    (ins_nullfunc, 0),		// 0x87
    (ins_nullfunc, 1),		// 0x88
    (nop_immediate, 2),		// 0x89
    (txa_implied, 1),		// 0x8A
    (ins_nullfunc, 0),		// 0x8B
    (ins_nullfunc, 0),		// 0x8C
    (sta_absolute, 1),		// 0x8D
    (ins_nullfunc, 0),		// 0x8E
    (ins_nullfunc, 0),		// 0x8F
////////////// 90 /////////////
    (ins_nullfunc, 1),		// 0x90
    (ins_nullfunc, 0),		// 0x91
    (ins_nullfunc, 0),		// 0x92
    (ins_nullfunc, 0),		// 0x93
    (ins_nullfunc, 0),		// 0x94
    (ins_nullfunc, 0),		// 0x95
    (ins_nullfunc, 0),		// 0x96
    (ins_nullfunc, 0),		// 0x97
    (ins_nullfunc, 1),		// 0x98
    (ins_nullfunc, 0),		// 0x99
    (txs_implied, 1),		// 0x9A
    (ins_nullfunc, 0),		// 0x9B
    (ins_nullfunc, 0),		// 0x9C
    (sta_absolute_ix, 1),	// 0x9D
    (ins_nullfunc, 0),		// 0x9E
    (ins_nullfunc, 0),		// 0x9F
////////////// A0 /////////////
    (ins_nullfunc, 0),		// 0xA0
    (ins_nullfunc, 0),		// 0xA1
    (ldx_immediate, 1),		// 0xA2
    (ins_nullfunc, 0),		// 0xA3
    (ins_nullfunc, 0),		// 0xA4
    (ins_nullfunc, 0),		// 0xA5
    (ins_nullfunc, 0),		// 0xA6
    (ins_nullfunc, 0),		// 0xA7
    (ins_nullfunc, 0),		// 0xA8
    (lda_immediate, 1),		// 0xA9
    (ins_nullfunc, 0),		// 0xAA
    (ins_nullfunc, 0),		// 0xAB
    (ins_nullfunc, 0),		// 0xAC
    (lda_absolute, 1),		// 0xAD
    (ins_nullfunc, 0),		// 0xAE
    (ins_nullfunc, 0),		// 0xAF
////////////// B0 /////////////
    (ins_nullfunc, 0),		// 0xB0
    (ins_nullfunc, 0),		// 0xB1
    (ins_nullfunc, 0),		// 0xB2
    (ins_nullfunc, 0),		// 0xB3
    (ins_nullfunc, 0),		// 0xB4
    (ins_nullfunc, 0),		// 0xB5
    (ins_nullfunc, 0),		// 0xB6
    (ins_nullfunc, 0),		// 0xB7
    (ins_nullfunc, 0),		// 0xB8
    (ins_nullfunc, 0),		// 0xB9
    (ins_nullfunc, 0),		// 0xBA
    (ins_nullfunc, 0),		// 0xBB
    (ins_nullfunc, 0),		// 0xBC
    (lda_absolute_ix, 1),	// 0xBD
    (ins_nullfunc, 0),		// 0xBE
    (ins_nullfunc, 0),		// 0xBF
////////////// C0 /////////////
    (ins_nullfunc, 0),		// 0xC0
    (ins_nullfunc, 0),		// 0xC1
    (nop_immediate, 2),		// 0xC2
    (ins_nullfunc, 0),		// 0xC3
    (ins_nullfunc, 0),		// 0xC4
    (ins_nullfunc, 0),		// 0xC5
    (ins_nullfunc, 0),		// 0xC6
    (ins_nullfunc, 0),		// 0xC7
    (ins_nullfunc, 0),		// 0xC8
    (ins_nullfunc, 0),		// 0xC9
    (dex_implied, 1),		// 0xCA
    (ins_nullfunc, 0),		// 0xCB
    (ins_nullfunc, 0),		// 0xCC
    (ins_nullfunc, 0),		// 0xCD
    (ins_nullfunc, 0),		// 0xCE
    (ins_nullfunc, 0),		// 0xCF
////////////// D0 /////////////
    (bne_relative, 1),		// 0xD0
    (ins_nullfunc, 0),		// 0xD1
    (ins_nullfunc, 0),		// 0xD2
    (ins_nullfunc, 0),		// 0xD3
    (nop_immediate, 4),		// 0xD4
    (ins_nullfunc, 0),		// 0xD5
    (ins_nullfunc, 0),		// 0xD6
    (ins_nullfunc, 0),		// 0xD7
    (cld_implied, 1),		// 0xD8
    (ins_nullfunc, 0),		// 0xD9
    (nop_immediate, 2),		// 0xDA
    (ins_nullfunc, 0),		// 0xDB
    (nop_immediate, 4),		// 0xDC
    (cmp_absolute_ix, 1),	// 0xDD
    (ins_nullfunc, 0),		// 0xDE
    (ins_nullfunc, 0),		// 0xDF
////////////// E0 /////////////
    (ins_nullfunc, 0),		// 0xE0
    (ins_nullfunc, 0),		// 0xE1
    (nop_immediate, 2),		// 0xE2
    (ins_nullfunc, 0),		// 0xE3
    (ins_nullfunc, 0),		// 0xE4
    (ins_nullfunc, 0),		// 0xE5
    (ins_nullfunc, 0),		// 0xE6
    (ins_nullfunc, 0),		// 0xE7
    (inx_implied, 1),		// 0xE8
    (ins_nullfunc, 0),		// 0xE9
    (nop_immediate, 2),		// 0xEA
    (ins_nullfunc, 0),		// 0xEB
    (ins_nullfunc, 0),		// 0xEC
    (ins_nullfunc, 0),		// 0xED
    (ins_nullfunc, 0),		// 0xEE
    (ins_nullfunc, 0),		// 0xEF
////////////// F0 /////////////
    (beq_relative, 0),		// 0xF0
    (ins_nullfunc, 0),		// 0xF1
    (ins_nullfunc, 0),		// 0xF2
    (ins_nullfunc, 0),		// 0xF3
    (nop_immediate, 4),		// 0xF4
    (ins_nullfunc, 0),		// 0xF5
    (ins_nullfunc, 0),		// 0xF6
    (ins_nullfunc, 0),		// 0xF7
    (ins_nullfunc, 0),		// 0xF8
    (ins_nullfunc, 0),		// 0xF9
    (nop_immediate, 2),		// 0xFA
    (ins_nullfunc, 0),		// 0xFB
    (nop_immediate, 4),		// 0xFC
    (ins_nullfunc, 0),		// 0xFD
    (ins_nullfunc, 0),		// 0xFE
    (ins_nullfunc, 0),		// 0xFF
];