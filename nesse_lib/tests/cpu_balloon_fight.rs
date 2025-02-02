use nesse_lib::system::nes::NES;

#[test]
fn test_entry_point() {

    let mut nes_emu = NES::new();

    nes_emu.insert_rom(&"tests/test_ines/Balloon Fight (USA).nes").expect("Could not load ROM");
    assert_eq!(nes_emu.bus.memory.read::<u16>(0xFFFC).unwrap(), 0xC000);
    assert_eq!(nes_emu.bus.memory.read::<u16>(0xFFFE).unwrap(), 0xC0F7);
}