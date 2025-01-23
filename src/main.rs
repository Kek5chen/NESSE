use crate::system::nes::NES;

mod system;

fn main() {
    env_logger::init();

    let mut nes_emu = NES::new();

    nes_emu.insert_rom(&"Balloon Fight (USA).nes").expect("Could not load ROM");
    nes_emu.run().unwrap();
}