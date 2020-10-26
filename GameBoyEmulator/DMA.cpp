#include "DMA.h"
#include "Bus.h"
DMA::DMA(Bus* bus) {
	this->bus = bus;
	is_running = false;
	remaining_clocks = 0;
}
void DMA::startDMA() {
	curr_addr = bus->read(0x46);
	printf("DMA started at PC : %04X, from address %04X\n", bus->cpu.reg.PC, curr_addr);
	if (curr_addr > 0xF1) return;
	curr_addr <<= 8;
	is_running = true;
	remaining_clocks = 160;

}

void DMA::clock() {
	if (!is_running) return;
	uint8_t data;
	if (curr_addr < 0x8000 || (0xA000 <= curr_addr && curr_addr < 0xC000)) {// from cartridge ROM or external RAM
		data = bus->cartridge->read(curr_addr);
	}
	else if (0x8000 <= curr_addr && curr_addr < 0xA000) {// from VRAM
		data = bus->ppu.read(curr_addr);
	}
	else if (0xC000 <= curr_addr && curr_addr < 0xFE00) {// from internal RAM
		data = bus->cpuRam[curr_addr & 0x0FFF];
	}
	uint8_t oam_addr = 160 - remaining_clocks;
	bus->ppu.oam[oam_addr] = data;

	remaining_clocks--;
	curr_addr++;
	if (remaining_clocks == 0) is_running = false;
}