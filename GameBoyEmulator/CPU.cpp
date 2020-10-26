#include "CPU.h"
#include "Bus.h"

void CPU::connectBus(Bus* b) {
	this->bus = b;
	reset();
}

uint8_t CPU::getFlag(CPU::flags f) {
	return (reg.F & f)>0;
}

void CPU::setFlag(CPU::flags f, bool v) {
	if (v) {
		reg.F |= f;
	}
	else {
		reg.F &= ~f;
	}
}

uint8_t CPU::read(uint16_t addr) {
	return bus->read(addr);
}

void CPU::write(uint16_t addr, uint8_t data) {
	bus->write(addr, data);
}

void CPU::clock() {
	if (cycle == 0) {
		uint8_t opcode = read(reg.PC++);
		cycle = exec_opcode(opcode);
	}
	clock_count++;
	cycle--;
}

void CPU::reset() {
	reg.A = 0x01;
	reg.F = 0xB0;
	reg.B = 0x00;
	reg.C = 0x13;
	reg.D = 0x00;
	reg.E = 0xD8;
	reg.H = 0x01;
	reg.L = 0x4D;
	reg.SP = 0xFFFE;
	reg.PC = 0x100;
	bus->write(0xFFFF, 0x00);
}
void CPU::handle_intr() {
	if (IME) {
		uint8_t IE = read(0xFFFF); // Get Interrupt Enable flag
		uint8_t IF = read(0xFF0F); // Get Interrupt Flag

		if (IE & IF) 
		{
			is_halt = 0;

			//following the way Z80 execute interruption.. (yet to be tested)
			cycle += 5;
		}

		if ((IE & IF & 0x01)) { // V-Blank Interrupt 
			IF &= ~(0x01); // Reset the flag
			IME = 0; // Clear IME flag
			write(--reg.SP, reg.PC >> 8); //push the PC to stack
			write(--reg.SP, (reg.PC & 0x00FF));
			reg.PC = 0x0040; //jump to interrupt location
			write(0xFF0F, IF);
		}

		else if ((IE & IF & 0x02)) { // LCD STAT Interrupt 
			IF &= ~(0x02); 
			IME = 0; 
			write(--reg.SP, reg.PC >> 8); 
			write(--reg.SP, (reg.PC & 0x00FF));
			reg.PC = 0x0048;
			write(0xFF0F, IF);
		}
		else if ((IE & IF & 0x04)) { // Timer Interrupt 
			IF &= ~(0x04);
			IME = 0;
			write(--reg.SP, reg.PC >> 8);
			write(--reg.SP, (reg.PC & 0x00FF));
			reg.PC = 0x0050;
			write(0xFF0F, IF);
		}
		else if ((IE & IF & 0x08)) { // Serial Interrupt 
			IF &= ~(0x08);
			IME = 0;
			write(--reg.SP, reg.PC >> 8);
			write(--reg.SP, (reg.PC & 0x00FF));
			reg.PC = 0x0058;
			write(0xFF0F, IF);
		}
		else if ((IE & IF & 0x10)) { // Joypad Interrupt 
			IF &= ~(0x10);
			IME = 0;
			write(--reg.SP, reg.PC >> 8);
			write(--reg.SP, (reg.PC & 0x00FF));
			reg.PC = 0x0060;
			write(0xFF0F, IF);
		}
	}
}
