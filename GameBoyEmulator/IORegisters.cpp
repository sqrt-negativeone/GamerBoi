#include "IORegisters.h"
#include "Bus.h"
/*
	I used the informations I found in this website to implement the behavior of IO registers
	https://mgba-emu.github.io/gbdoc/
*/

IORegisters::IORegisters() {
	//do init stuff
	reset();
}
void IORegisters::reset()
{
	
	io_registers[0x05] = 0x00;//TIMA
	io_registers[0x06] = 0x00;//TMA
	io_registers[0x07] = 0x00;//TAC
	io_registers[0x10] = 0x80;//NR10
	io_registers[0x11] = 0xBF;//NR11
	io_registers[0x12] = 0xF3;//NR12
	io_registers[0x14] = 0xBF;//NR14
	io_registers[0x16] = 0x3F;//NR21
	io_registers[0x17] = 0x00;//NR22
	io_registers[0x19] = 0xBF;//NR24
	io_registers[0x1A] = 0x7F;//NR30
	io_registers[0x1B] = 0xFF;//NR31
	io_registers[0x1C] = 0x9F;//NR32
	io_registers[0x1E] = 0xBF;//NR33
	io_registers[0x20] = 0xFF;//NR41
	io_registers[0x21] = 0x00;//NR42
	io_registers[0x22] = 0x00;//NR43
	io_registers[0x23] = 0xBF;//NR44
	io_registers[0x24] = 0x77;//NR50
	io_registers[0x25] = 0xF3;//NR51
	io_registers[0x26] = 0xF1;//NR52
	io_registers[0x40] = 0x91;//LCDC
	io_registers[0x42] = 0x00;//SCY
	io_registers[0x43] = 0x00;//SCX
	io_registers[0x44] = 0x00;//LY
	io_registers[0x45] = 0x00;//LYC
	io_registers[0x47] = 0xFC;//BGP
	io_registers[0x48] = 0xFF;//OBP0
	io_registers[0x49] = 0xFF;//OBP1
	io_registers[0x4A] = 0x00;//WY
	io_registers[0x4B] = 0x00;//WX
}
void IORegisters::connectBus(Bus* bus) {
	this->bus = bus;
}
uint8_t IORegisters::read(uint16_t addr) {
	switch (addr)
	{
	case 0xFF00: {//Joypad
		return bus->joypad.read();;
	}
	case 0xFF04: { //DIV : clock divider
		return bus->timer.read_DIV();
	}
	case 0xFF05: {//TIMA 
		return io_registers[addr - 0xFF00];
	}
	case 0xFF06: {//TMA
		return io_registers[addr - 0xFF00];
	}
	case 0xFF07: {//TAC
		return (0xF8 | (io_registers[0x07] & 0x07));
	}
	case 0xFF0F: {//IF 
		return (0xE0 | (io_registers[0x0F] & 0x1F));
	}
	case 0xFF40: {//LCDC
		return io_registers[addr - 0xFF00];
	}
	case 0xFF41: {//STAT
		uint8_t mode = bus->ppu.get_mode();
		uint8_t coincidence = bus->ppu.get_coincidence();
		uint8_t old = io_registers[0x41];
		io_registers[0x41] = (0x80 | (old & 0x78) | (mode | (coincidence << 2)));
		return io_registers[0x41];
	}
	case 0xFF42://SCY
	case 0xFF43://SCX
	case 0xFF44://LY
	case 0xFF45://LYC
	case 0xFF46://DMA
	case 0xFF47://BGP
	case 0xFF48://OBP0
	case 0xFF49://OBP1
	case 0xFF4A://WY
	case 0xFF4B://WX
	{
		return io_registers[addr - 0xFF00];
		break;
	}
	default://Unmapped or not implemented yey
		break;
	}
	return 0xFF;
}

void IORegisters::write(uint16_t addr, uint8_t data) {
	switch (addr)
	{
	case 0xFF00: {//Joypad
		bus->joypad.write(data);
		break;
	}
	case 0xFF04: { //DIV : clock divider
		//writing to this register resets it to zero
		bus->timer.resetDIV();
		break;
	}
	case 0xFF05: {//TIMA 
		io_registers[addr - 0xFF00] = data;
		break;
	}
	case 0xFF06: {//TMA
		io_registers[addr - 0xFF00] = data;
		break;
	}
	case 0xFF07: {//TAC
		uint8_t old_freq = bus->timer.get_clockFreq();
		io_registers[0x07] = (0xF8 | (data & 0x07));
		uint8_t new_freq = bus->timer.get_clockFreq();
		if (new_freq != old_freq) {
			bus->timer.reset_timer_ticks();
		}
		break;
	}
	case 0xFF0F: {//IF 
		io_registers[0x0F] = (0xE0 | (data & 0x1F));
		break;
	}
	case 0xFF40: {//LCDC
		io_registers[addr - 0xFF00] = data;
		if ((data & (1 << 0)) == 0) {
			//the value of the bit 5 is overriden by the bit 0 if that bit is reset
			data &= ~(1 << 5);
		}
		if ((data & (1 << 7)) == 0) {
			//turning off the screen
			io_registers[0x44] = 0;
			bus->ppu.turnedOff();
		}
		break;
	}
	case 0xFF41: {//STAT
		uint8_t mode = bus->ppu.get_mode();
		uint8_t coincidence = bus->ppu.get_coincidence();
		uint8_t old = io_registers[0x41];
		io_registers[0x41] = (0x80 | (data & 0x78) | (mode | (coincidence << 2)));
		break;
	}
	case 0xFF42: 
	case 0xFF43: {//SCY and SCX
		io_registers[addr-0xFF00] = data;
		break;
	}
	case 0xFF44: {//LY
		//readonly, reset to 0 if write to
		io_registers[0x44] = 0;
		break;
	}
	case 0xFF45: {//LYC
		io_registers[addr - 0xFF00] = data;
		break;
	}
	case 0xFF46: {//DMA
		io_registers[addr - 0xFF00] = data;
		bus->ppu.dma->startDMA();
		break;
	}
	case 0xFF47:
	case 0xFF48:
	case 0xFF49:
	case 0xFF4A:
	case 0xFF4B:{//BGP, OBP0, OBP1, WY, WX
		io_registers[addr - 0xFF00] = data;
		break;
	}
	default://Unmapped or not implemented yey
		break;
	}
}

void IORegisters::inc_LY() {
	io_registers[0x44]++;
}
void IORegisters::inc_DIV() {
	io_registers[0x04]++;
}