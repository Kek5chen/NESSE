use crate::system::nes::NES;

pub struct Debugger<'a> {
    nes: &'a NES,
}

impl<'a> Debugger<'a> {
    pub fn new(nes: &'a NES) -> Debugger<'a> {
        Debugger { nes }
    }
}