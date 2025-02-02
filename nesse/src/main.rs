use nesse_lib::system::nes::NES;


fn main() {
    env_logger::init();

    let mut nes_emu = NES::new();

    nes_emu.insert_rom(&"nesse_lib/tests/test_ines/Balloon Fight (USA).nes").expect("Could not load ROM");
    nes_emu.run().unwrap();
}