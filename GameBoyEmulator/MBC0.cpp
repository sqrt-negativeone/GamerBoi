#include "MBC0.h"
MBC0::MBC0(Cartridge* cart) : MBC(cart) {}

uint8_t MBC0::read(uint16_t addr) {
    if (addr < 0x8000) { // no memory banking
        return cart->rom[addr];
    }
    if (cart->ram != nullptr && 0xA000 <= addr && addr < 0xC000) {
        return cart->ram[addr];
    }
    return 0;
}
void MBC0::write(uint16_t addr, uint8_t data) {//readonly
    if (cart->ram != nullptr && 0xA000 <= addr && addr < 0xC000) {
        cart->ram[addr] = data;
    }
    return;
}