#include "Bus.h"
#include <conio.h>
Bus::Bus() {
	is_booting = true;

	ppu.connectBus(this);
	cpu.connectBus(this);
	timer.connectBus(this);
	io_registers.connectBus(this);

	memset(cpuRam, 0x00, sizeof(cpuRam));
	memset(h_ram, 0x00, sizeof(h_ram));
	FILE* file;
	fopen_s(&file, "ROMs/dmg_boot.bin", "rb");
	uint8_t pos = 0;
	while (fread(&boot_rom[pos], 1, 1, file)) pos++;
}

Bus::~Bus(){}

uint8_t Bus::read(uint16_t addr) {
	if (ppu.dma->is_running && !(0xFF80 <= addr && addr < 0xFFFF)) {// don't read when DMA transfer
		return 0xFF;
	}
	if ((0x0000 <= addr && addr < 0x8000) || (0xA000 <= addr && addr < 0xC000)) {//read from ROM or external RAM
		if (is_booting && addr == 0x100) is_booting = false;
		if (is_booting && 0x0000 <= addr && addr < 0x0100) {// read from boot rom instead
			return boot_rom[addr];
		}
		if (cartridge) {
			return cartridge->read(addr);
		}
		else return 0x00;
	}
	if ((0x8000 <= addr && addr < 0xA000) || (0xFE00 <= addr && addr < 0xFEA0)) {//read from VRAM or OAM tables
		return ppu.read(addr);
	}
	if (0xFF00 <= addr && addr < 0xFF80) {//IO registers
		return io_registers.read(addr);
	}
	if (0xFF80 <= addr && addr <= 0xFFFE) {//read from HRAM
		return h_ram[addr - 0xFF80];
	} 
	if (addr == 0xFFFF) {//IE register
		return cpu.read_IE_flag();
	}
	if (0xE000 <= addr && addr < 0xFE00) {//mirror of C000-CFFF
		return read(addr - 0x2000);
	}
	if (0xFEA0 <= addr && addr < 0xFF00) {// not usable
		return 0xFF;
	}
	if (0xC000 <= addr && addr < 0xE000) {//read from RAM
		return cpuRam[addr - 0xC000];
	}
}
void Bus::write(uint16_t addr, uint8_t data) {
	
	if (ppu.dma->is_running && !(0xFF80 <= addr && addr < 0xFFFF)) {// don't write when DMA transfer
		return;
	}
	//write to ROM or external RAM (writing to rom causes the MBC to change banks)
	if ((0x0000 <= addr && addr < 0x8000) || (0xA000 <= addr && addr < 0xC000)) {
		if (is_booting) return;
		if (cartridge) cartridge->write(addr, data);
		return;
	}

	if ((0x8000 <= addr && addr < 0xA000) || (0xFE00 <= addr && addr < 0xFEA0)) {//write to VRAM or OAM tables
		ppu.write(addr, data);
		return;
	}
	if (addr == 0xFFFF) {//IE register
		cpu.write_IE_flag(data);
		return;
	}
	if (0xFF00 <= addr && addr < 0xFF80) {//IO registers
		io_registers.write(addr, data);
		return;
	}
	if (0xFF80 <= addr && addr < 0xFFFF) {//write to HRAM
		h_ram[addr - 0xFF80] = data;
		return;
	}

	if (0xE000 <= addr && addr < 0xFE00) {//mirror of C000-CFFF
		write(addr - 0x2000,data);
		return;
	}

	if (0xFEA0 <= addr && addr < 0xFF00) {// not usable
		return;
	}
	if (0xC000 <= addr && addr < 0xE000) {// write to RAM
		cpuRam[addr - 0xC000] = data; 
	}
}

void Bus::clock() {
	cpu.clock();
	ppu.clock();
	timer.update();
	cpu.handle_intr();
}

void Bus::interrupt_req(uint8_t req) {
	uint8_t IF = io_registers.read(0xFF0F);
	io_registers.write(0xFF0F, IF | req);
}

void Bus::insertCartridge(Cartridge* cartridge) {
	this->cartridge = cartridge;
}

void Bus::reset() { 
	cpu.reset();
	io_registers.reset();
	ppu.reset();
	timer.reset();
}