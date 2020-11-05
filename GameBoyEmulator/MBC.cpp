#include "MBC.h"
namespace GamerBoi {
	MBC::MBC(Cartridge* cart) {
		this->cart = cart;
		total_rom_banks = (cart->rom.size()) / 0x4000;
		if (cart->ram)
			ram_size = (sizeof(cart->ram) / sizeof(cart->ram[0]));
	}

	uint8_t MBC::read(uint16_t addr) {
		return 0;
	}

	void MBC::write(uint16_t addr, uint8_t data) {
		//will be overriden
	}
}