#pragma once
#include "Cartridge.h"
namespace GamerBoi {
	class MBC
	{
	public:
		MBC(Cartridge* cart);
 		virtual uint8_t read(uint16_t addr);
		virtual void write(uint16_t addr, uint8_t data);
	protected:
		uint8_t bank1 = 1;
		uint8_t bank2 = 0;
		bool is_ram_enable = 0;
		Cartridge* cart;
		uint8_t total_rom_banks = 0;
		uint8_t ram_size = 0;
	};

}
