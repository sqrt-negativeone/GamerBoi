#include "MBC.h"
namespace GamerBoi {
	MBC::MBC(Cartridge* cart) {
		this->cart = cart;
		total_rom_banks = (cart->rom.size()) / 0x4000;
	}

	uint8_t MBC::read(uint16_t addr) {
		return 0;
	}

	void MBC::write(uint16_t addr, uint8_t data) {
		//will be overriden
	}
	void MBC::set_ram_size(uint16_t ram_size) {
		this->ram_size = ram_size;
	}
}