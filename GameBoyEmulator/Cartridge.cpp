#include "Cartridge.h"
#include "MBC.h"
#include "MBC0.h"
#include "MBC1.h"
namespace GamerBoi {
	Cartridge::Cartridge(const char* path) {
		FILE* file;
		fopen_s(&file, path, "rb");
		uint8_t data;
		while (fread(&data, 1, 1, file)) {
			rom.push_back(data);
		}
	
		uint8_t cart_type = rom[0x0147];
		switch (cart_type) {
		case 0x08:
		case 0x00: {
			mbc = new MBC0(this);
			break;
		}
		case 0x01:
		case 0x02:
		case 0x03: {
			mbc = new MBC1(this);
			break;
		}
		//case 0x05:
		//case 0x06: {
		//	//TODO: add mbc2
		//	break;
		//}
		default: {
			printf("error, MBC not implemented yet, type : %X\n", cart_type);
		}
		}
	
		uint8_t ram_type = rom[0x0149];
		switch (ram_type) {
		case 0x00: break;
		case 0x01: ram = new uint8_t[(1<<11)]; break;
		case 0x02: ram = new uint8_t[(1 << 13)]; break;
		case 0x03: ram = new uint8_t[(1<<15)]; break;
		case 0x04: ram = new uint8_t[(1 << 17)]; break;
		case 0x05: ram = new uint8_t[(1 << 16)]; break;
		}
	}
	Cartridge::~Cartridge() {
		delete mbc;
		delete ram;
	}
	uint8_t Cartridge::read(uint16_t addr) {
		return mbc->read(addr);
	}

	void Cartridge::write(uint16_t addr, uint8_t data) {
		mbc->write(addr, data);
	}
}

