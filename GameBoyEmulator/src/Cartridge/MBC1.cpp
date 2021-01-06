#include "MBC1.h"
namespace GamerBoi {
	MBC1::MBC1(Cartridge* cart) : MBC(cart),
		mode(0)
	{}

	uint8_t MBC1::read(uint16_t addr) {
		if (addr < 0x4000) { //read from ROM bank 00
			if (mode == 0) 	return cart->rom[addr];
			uint32_t zero = 0;
			if (total_rom_banks <= 32) zero = 0;
			else if (total_rom_banks == 64) zero = ((bank2 & 1) << 5);
			else if (total_rom_banks == 128) zero = (bank2 << 5);
			uint32_t real_addr = 0x4000 * zero + (uint32_t)addr;
			return cart->rom[real_addr % cart->rom.size()];
		}
		else if (addr < 0x8000) { //read from ROM bank NN
			uint8_t high = ((bank2 << 5) | bank1);
			//if (total_rom_banks <= 32) {
			//	high = bank1;
			//}
			//else if (total_rom_banks == 64) {
			//	high = (((bank2 & 1) << 5) | bank1);
			//}
			//else if (total_rom_banks == 128) {
			//	high = ((bank2 << 5) | bank1);
			//}
			uint32_t real_addr = 0x4000 * (high - 1) + (uint32_t)addr;
			return cart->rom[real_addr];
		}
		else if (cart->ram && is_ram_enable && 0xA000 <= addr && addr < 0xC000) {//read from RAM bank NN
			if (ram_size <= 0x2000) return cart->ram[(addr - 0xA000) % ram_size];
			return (mode) ? cart->ram[(0x2000 * bank2 + addr - 0xA000) % ram_size] : cart->ram[(addr - 0xA000) % ram_size];
		}
		return 0xFF;// ram disable or reading from somewhere else.. idk
	}

	void MBC1::write(uint16_t addr, uint8_t data) {
		if (addr < 0x2000) {// RAM enable
			is_ram_enable = ((data & 0x0F) == 0x0A);
		}
		else if (addr < 0x4000) {//BANK1 register
			/*switch (data)
			{
			case 4: data &= 0x03; break;
			case 8: data &= 0x07; break;
			case 16: data &= 0xF; break;
			case 32:
			case 64:
			case 128:
				data &= 0x1F; break;
			}
			if (data == 0) data++;*/
			data &= 0x1F;
			bank1 = data;
			if (bank1 == 0) bank1++;
		}
		else if (addr < 0x6000) {//BANK2 register
			data &= 0x03;
			bank2 = data;
		}
		else if (addr < 0x8000) {//MODE register
			mode = (data & 1);
		}
		if (cart->ram && is_ram_enable && 0xA000 <= addr && addr < 0xC000) {
			if (ram_size <= 0x2000) cart->ram[(addr - 0xA000) % ram_size] = data;
			else {
				if (mode) cart->ram[(0x2000 * bank2 + addr - 0xA000) % ram_size] = data;
				else cart->ram[(addr - 0xA000) % ram_size] = data;
			}
		}
	}

}