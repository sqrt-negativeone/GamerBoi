#include "CPU.h"
#include "../Bus/Bus.h"
namespace GamerBoi {
	void CPU::connectBus(Bus* b) {
		this->bus = b;
		reset();
	}

	void CPU::reset() {
		reg.AF.hl = 0x01B0;
		reg.BC.hl = 0x0013;
		reg.DE.hl = 0x00D8;
		reg.HL.hl = 0x014D;
		reg.SP.hl = 0xFFFE;
		reg.PC = 0x0000;
		write_IE_flag(0);
		write_IF_flag(0);
		IME = 0;
		bus->write(0xFFFF, 0x00);
	}

	uint8_t CPU::read_IE_flag() {
		return IE.val;
	}
	void CPU::write_IE_flag(uint8_t data) {
		IE.val = data;
		IE._ = 0;
	}
	uint8_t CPU::read_IF_flag() {
		return IF.val;
	}
	void CPU::write_IF_flag(uint8_t data) {
		IF.val = data;
		IF._ = 0x7;
	}

	uint8_t CPU::read(uint16_t addr) {
		return bus->read(addr);
	}

	void CPU::write(uint16_t addr, uint8_t data) {
		bus->write(addr, data);
	}

	uint8_t CPU::clock() {
		if (is_halt) cycle = 1;
		else {
			if (halt_bug) {
				op.opcode = read(reg.PC);
				halt_bug = false;
			}
			else op.opcode = read(reg.PC++);

 			cycle = exec_opcode();
		}

		handle_intr();

		return cycle;
	}


	void CPU::handle_intr() {
		
	
		if (IME) {

			if (IE.val & IF.val & 0x1F)
			{
				is_halt = 0;

				//following the way Z80 execute interruption.. (yet to be tested)
				cycle += 5;
			}
			else return;

			if (IE.vblank & IF.vblank) { // V-Blank Interrupt 
				IF.vblank = 0; // Reset the flag
				IME = 0; // Clear IME flag
				write(--reg.SP.hl, reg.PC >> 8); //push the PC to stack
				write(--reg.SP.hl, (reg.PC & 0x00FF));
				reg.PC = 0x0040; //jump to interrupt location
			}

			else if (IE.lcd_stat & IF.lcd_stat) { // LCD STAT Interrupt 
				IF.lcd_stat = 0;
				IME = 0;
				write(--reg.SP.hl, reg.PC >> 8);
				write(--reg.SP.hl, (reg.PC & 0x00FF));
				reg.PC = 0x0048;
			}
			else if (IE.timer & IF.timer) { // Timer Interrupt 
				IF.timer = 0;
				IME = 0;
				write(--reg.SP.hl, reg.PC >> 8);
				write(--reg.SP.hl, (reg.PC & 0x00FF));
				reg.PC = 0x0050;
			}
			else if (IE.serial & IF.serial) { // Serial Interrupt 
				IF.serial = 0;
				IME = 0;
				write(--reg.SP.hl, reg.PC >> 8);
				write(--reg.SP.hl, (reg.PC & 0x00FF));
				reg.PC = 0x0058;
			}
			else if (IE.joypad & IF.joypad) { // Joypad Interrupt 
				IF.joypad = 0;
				IME = 0;
				write(--reg.SP.hl, reg.PC >> 8);
				write(--reg.SP.hl, (reg.PC & 0x00FF));
				reg.PC = 0x0060;
			}
		}
		else {
			if (is_halt && (IE.val & IF.val & 0x1F)) {
				is_halt = false;
			}
		}
	}

	uint8_t CPU::exec_opcode() {
		uint8_t cycles = 0;
		uint8_t p = (op.y >> 1);
		uint8_t q = (op.y & 1);
		switch (op.opcode) {
		case 0x00:
		{ //NOP
			cycles = 1;
			break;
		}
		case 0x08:
		{ //LD (a16), SP
			cycles = 5;
			uint16_t l = read(reg.PC++);
			uint16_t h = read(reg.PC++);
			uint16_t addr = ((h << 8) | l);
			write(addr++, reg.SP.l);
			write(addr, reg.SP.h);
			break;
		}
		case 0x10:
		{//STOP
			cycles = 1;
			is_stop = 1;
			break;
		}
		case 0x18:
		{//JR e
			cycles = 3;
			int8_t data = (int8_t)read(reg.PC++);
			reg.PC += data;
			break;
		}
		case 0x20: case 0x30: case 0x28: case 0x38:
		{//JR cc,d
			cycles = 2;
			int8_t data = (int8_t)read(reg.PC++);
			if ((this->*cc[op.y - 4])()) {
				cycles++;
				reg.PC += data;
			}
			break;
		}
		case 0x01: case 0x11: case 0x21: case 0x31:
		{// LD rp[p],nn
			cycles = 3;
			uint8_t l = read(reg.PC++);
			uint16_t h = read(reg.PC++);
			*rp[p] = ((h << 8) | l);
			break;
		}
		case 0x09: case 0x19: case 0x29: case 0x39:
		{// ADD HL,rp[p]
			cycles = 2;

			uint16_t tmp1 = *rp[p];
			uint16_t tmp2 = reg.HL.hl;
			uint16_t ans = tmp1 + tmp2;

			reg.HL.hl = ans;

			//set flags
			reg.AF.N = 0;
			reg.AF.H = ((((tmp1 & 0x0FFF) + (tmp2 & 0x0FFF)) & 0x1000) > 0);
			reg.AF.C = ((((tmp1 & 0x0FFFF) + (tmp2 & 0x0FFFF)) & 0x10000) > 0);
			break;
		}
		case 0x02:
		{// LD (BC),A
			cycles = 2;
			write(reg.BC.hl, reg.AF.h);
			break;
		}
		case 0x12:
		{// LD (DE),A
			cycles = 2;
			write(reg.DE.hl, reg.AF.h);
			break;
		}
		case 0x22:
		{//LD (HL+),A
			cycles = 2;
			write(reg.HL.hl++, reg.AF.h);
			break;

		}
		case 0x32:
		{//LD (HL-),A
			cycles = 2;
			write(reg.HL.hl--, reg.AF.h);
			break;
		}
		case 0x0A:
		{//LD A,(BC)
			cycles = 2;
			reg.AF.h = read(reg.BC.hl);
			break;
		}
		case 0x1A:
		{//LD A,(DE)
			cycles = 2;
			reg.AF.h = read(reg.DE.hl);
			break;
		}
		case 0x2A:
		{//LD A,(HL+)
			cycles = 2;
			reg.AF.h = read(reg.HL.hl++);
			break;
		}
		case 0x3A:
		{//LD A,(HL-)
			cycles = 2;
			reg.AF.h = read(reg.HL.hl--);
			break;
		}
		case 0x03: case 0x13: case 0x23: case 0x33:
		{// INC rp[p]
			cycles = 2;
			(*rp[p])++;
			break;
		}
		case 0x0B: case 0x1B: case 0x2B: case 0x3B:
		{// DEC rp[p]
			cycles = 2;
			(*rp[p])--;
			break;
		}
		case 0x34:
		{// INC (HL)
			cycles = 3;
			uint8_t data = read(reg.HL.hl);
			reg.AF.H = ((((data & 0x0F) + 1) & 0x10) > 0);
			data++;
			reg.AF.Z = (data == 0);
			reg.AF.N = 0;
			write(reg.HL.hl, data);
			break;
		}
		case 0x04: case 0x14: case 0x24: case 0x0C: case 0x1C: case 0x2C: case 0x3C:
		{// INC r[y]
			cycles = 1;
			reg.AF.H = (((((*r[op.y]) & 0x0F) + 1) & 0x10) > 0);
			(*r[op.y])++;
			reg.AF.Z = (*r[op.y] == 0);
			reg.AF.N = 0;
			break;
		}
		case 0x35:
		{// DEC (HL)
			cycles = 3;
			uint8_t data = read(reg.HL.hl);
			reg.AF.H = ((((data & 0x0F) - 1) & 0x10) != 0);
			data--;
			reg.AF.Z = (data == 0);
			reg.AF.N = 1;
			write(reg.HL.hl, data);
			break;
		}
		case 0x05: case 0x15: case 0x25: case 0x0D: case 0x1D: case 0x2D: case 0x3D:
		{// DEC r[y]
			cycles = 1;
			reg.AF.H = (((((*r[op.y]) & 0x0F) - 1) & 0x10) != 0);
			(*r[op.y])--;
			reg.AF.Z = (*r[op.y] == 0);
			reg.AF.N = 1;
			break;
		}
		case 0x36:
		{// LD (HL),d8
			cycles = 3;
			uint8_t data = read(reg.PC++);
			write(reg.HL.hl, data);
			break;
		}
		case 0x06:
		case 0x16:
		case 0x26:
		case 0x0E:
		case 0x1E:
		case 0x2E:
		case 0x3E:
		{// LD r[y],d8
			cycles = 2;
			*r[op.y] = read(reg.PC++);
			break;
		}
		case 0x07:
		{// RLCA
			cycles = 1;
			uint8_t CY = ((reg.AF.h & 0x80) >> 7);
			reg.AF.C = CY;
			reg.AF.Z = 0;
			reg.AF.N = 0;
			reg.AF.H = 0;
			reg.AF.h = ((reg.AF.h << 1) | CY);
			break;
		}
		case 0x0F:
		{//RRCA
			cycles = 1;
			uint8_t CY = (reg.AF.h & 0x01);

			reg.AF.C = CY;
			reg.AF.Z = 0;
			reg.AF.N = 0;
			reg.AF.H = 0;

			reg.AF.h = ((reg.AF.h >> 1) | (CY << 7));
			break;
		}
		case 0x17:
		{//RLA
			cycles = 1;
			uint8_t CY = reg.AF.C;
			reg.AF.C = ((reg.AF.h & 0x80) > 0);
			reg.AF.Z = 0;
			reg.AF.N = 0;
			reg.AF.H = 0;

			reg.AF.h = ((reg.AF.h << 1) | CY);
			break;
		}
		case 0x1F:
		{//RRA
			cycles = 1;
			uint8_t CY = reg.AF.C;

			reg.AF.C = (reg.AF.h & 1);
			reg.AF.Z = 0;
			reg.AF.N = 0;
			reg.AF.H = 0;

			reg.AF.h = ((reg.AF.h >> 1) | (CY << 7));
			break;
		}
		case 0x27:
		{//DAA
			cycles = 1;
			uint16_t correction = reg.AF.C ? 0x60 : 0x00;
			if (reg.AF.H || (!reg.AF.N && ((reg.AF.h & 0x0F) > 0x09))) {
				correction |= 0x06;
			}
			if (reg.AF.C || (!reg.AF.N && (reg.AF.h > 0x99))) {
				correction |= 0x60;
			}
			if (reg.AF.N) {
				reg.AF.h = (reg.AF.h - correction);
			}
			else {
				reg.AF.h = (reg.AF.h + correction);
			}
			if (((correction << 2) & 0x100) != 0) {
				reg.AF.C = 1;
			}
			reg.AF.H = 0;
			reg.AF.Z = (reg.AF.h == 0);
			break;
		}
		case 0x2F:
		{//CPL
			cycles = 1;
			reg.AF.h = ~reg.AF.h;
			reg.AF.N = 1;
			reg.AF.H = 1;
			break;
		}
		case 0x37:
		{//SCF
			cycles = 1;
			reg.AF.C = 1;
			reg.AF.H = 0;
			reg.AF.N = 0;
			break;
		}
		case 0x3F:
		{//CCF
			cycles = 1;
			reg.AF.C ^= 1;
			reg.AF.H = 0;
			reg.AF.N = 0;
			break;
		}
		case 0x76:
		{//HALT
			cycles = 1;
			if (IME) {
				is_halt = 1;
			}
			else {
				uint8_t IE = read_IE_flag();
				uint8_t IF = read(0xFF0F);
				if (IE & IF & 0x1F) {
					halt_bug = true;
				}
				else {
					is_halt = 1;
				}
			}
			break;
		}
		case 0x40: case 0x41: case 0x42:case 0x43:case 0x44:case 0x45:case 0x46:case 0x47:case 0x48:case 0x49:case 0x4A:case 0x4B:case 0x4C:case 0x4D:case 0x4E:case 0x4F:
		case 0x50: case 0x51: case 0x52:case 0x53:case 0x54:case 0x55:case 0x56:case 0x57:case 0x58:case 0x59:case 0x5A:case 0x5B:case 0x5C:case 0x5D:case 0x5E:case 0x5F:
		case 0x60: case 0x61: case 0x62:case 0x63:case 0x64:case 0x65:case 0x66:case 0x67:case 0x68:case 0x69:case 0x6A:case 0x6B:case 0x6C:case 0x6D:case 0x6E:case 0x6F:
		case 0x70: case 0x71: case 0x72:case 0x73:case 0x74:case 0x75: case 0x77:case 0x78:case 0x79:case 0x7A:case 0x7B:case 0x7C:case 0x7D:case 0x7E:case 0x7F:
		{//LD r[y],r[z]
			cycles = 1;
			uint8_t data;
			if (r[op.z] == nullptr) {//r[z] = (HL)
				cycles++;
				data = read(reg.HL.hl);
			}
			else {
				data = *r[op.z];
			}
			if (r[op.y] == nullptr) {//r[y]= (HL)
				cycles++;
				write(reg.HL.hl, data);
			}
			else {
				*r[op.y] = data;
			}
			break;
		}
		case 0x80: case 0x81: case 0x82:case 0x83:case 0x84:case 0x85:case 0x86:case 0x87:case 0x88:case 0x89:case 0x8A:case 0x8B:case 0x8C:case 0x8D:case 0x8E:case 0x8F:
		case 0x90: case 0x91: case 0x92:case 0x93:case 0x94:case 0x95:case 0x96:case 0x97:case 0x98:case 0x99:case 0x9A:case 0x9B:case 0x9C:case 0x9D:case 0x9E:case 0x9F:
		case 0xA0: case 0xA1: case 0xA2:case 0xA3:case 0xA4:case 0xA5:case 0xA6:case 0xA7:case 0xA8:case 0xA9:case 0xAA:case 0xAB:case 0xAC:case 0xAD:case 0xAE:case 0xAF:
		case 0xB0: case 0xB1: case 0xB2:case 0xB3:case 0xB4:case 0xB5:case 0xB6:case 0xB7:case 0xB8:case 0xB9:case 0xBA:case 0xBB:case 0xBC:case 0xBD:case 0xBE:case 0xBF:
		{
			//alu[y] r[z]
			cycles = 1;
			uint8_t data;
			if (r[op.z] == nullptr) {// r[z]=(HL)
				cycles++;
				data = read(reg.HL.hl);
			}
			else {
				data = *r[op.z];
			}
			(this->*alu[op.y])(data);
			break;
		}
		case 0xC0: case 0xD0: case 0xC8: case 0xD8:
		{
			// RET cc[y]
			cycles = 2;
			if ((this->*cc[op.y])()) {
				cycles = 5;
				uint8_t l = read(reg.SP.hl++);
				uint16_t h = read(reg.SP.hl++);
				reg.PC = ((h << 8) | l);
			}
			break;
		}
		case 0xE0:
		{//LD (0xFF00+d8),A
			cycles = 3;
			uint8_t data = read(reg.PC++);
			uint16_t addr = 0xFF00 + data;
			write(addr, reg.AF.h);
			break;
		}
		case 0xE8:
		{//ADD SP,e
			cycles = 4;
			int8_t data = (int8_t)read(reg.PC++);
			int res = reg.SP.hl + data;
			reg.AF.Z = 0;
			reg.AF.N = 0;
			reg.AF.H = (((reg.SP.hl ^ data ^ (res & 0xFFFF)) & 0x10) == 0x10);
			reg.AF.C = (((reg.SP.hl ^ data ^ (res & 0xFFFF)) & 0x100) == 0x100);
			reg.SP.hl = (uint16_t)res;
			break;
		}
		case 0xF0:
		{//LD A,(0xFF00+d8)
			cycles = 3;
			uint8_t data = read(reg.PC++);
			uint16_t addr = 0xFF00 + data;
			reg.AF.h = read(addr);
			break;
		}
		case 0xF8:
		{// LD HL, (SP+d8)
			cycles = 3;
			int8_t data = (int8_t)read(reg.PC++);
			int res = reg.SP.hl + data;
			reg.AF.Z = 0;
			reg.AF.N = 0;
			reg.AF.H = (((reg.SP.hl ^ data ^ (res & 0xFFFF)) & 0x10) == 0x10);
			reg.AF.C = (((reg.SP.hl ^ data ^ (res & 0xFFFF)) & 0x100) == 0x100);
			reg.HL.hl = (uint16_t)res;
			break;
		}
		case 0xC1: case 0xD1: case 0xE1: case 0xF1:
		{// POP rp2[p]
			cycles = 3;
			uint8_t l = read(reg.SP.hl++);
			uint16_t h = read(reg.SP.hl++);
			*rp2[p] = ((h << 8) | l);
			if (p == 3) {
				reg.AF.unused = 0;
			}
			break;
		}
		case 0xC9: case 0xD9:
		{// RET/ RETI
			cycles = 4;
			uint8_t l = read(reg.SP.hl++);
			uint16_t h = read(reg.SP.hl++);
			reg.PC = ((h << 8) | l);
			if (p) IME = 1;
			break;
		}
		case 0xE9:
		{//JP HL
			cycles = 1;
			reg.PC = reg.HL.hl;
			break;
		}
		case 0xF9:
		{//LD SP,HL
			cycles = 2;
			reg.SP.hl = reg.HL.hl;
			break;
		}
		case 0xC2: case 0xD2: case 0xCA: case 0xDA:
		{// JP cc[y], nn
			cycles = 3;
			uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
			if ((this->*cc[op.y])()) {
				cycles++;
				reg.PC = addr;
			}
			break;
		}
		case 0xE2:
		{//LD (0xFF00+C),A
			cycles = 2;
			uint16_t addr = 0xFF00 + reg.BC.l;
			write(addr, reg.AF.h);
			break;
		}
		case 0xEA:
		{//LD (a16),A
			cycles = 4;
			uint8_t l = read(reg.PC++);
			uint16_t h = read(reg.PC++);
			uint16_t addr = ((h << 8) | l);
			write(addr, reg.AF.h);
			break;
		}
		case 0xF2:
		{//LD A,(0xFF00+C)
			cycles = 2;
			reg.AF.h = read(0xFF00 + reg.BC.l);
			break;
		}
		case 0xFA:
		{//LD A,(a16)
			cycles = 4;
			uint8_t l = read(reg.PC++);
			uint16_t h = read(reg.PC++);
			uint16_t addr = ((h << 8) | l);
			reg.AF.h = read(addr);
			break;
		}
		case 0xC3:
		{// JP a16
			cycles = 4;
			uint8_t l = read(reg.PC++);
			uint16_t h = read(reg.PC++);
			reg.PC = (l | (h << 8));
			break;
		}
		case 0xCB:
		{//CB prefix
			op.opcode = read(reg.PC++);
			cycles = exec_prefixed();
			break;
		}
		case 0xF3:
		{//DI
			cycles = 1;
			//ime_disable_waiting = 0;
			IME = 0;
			break;
		}
		case 0xFB:
		{//EI
			cycles = 1;
			//ime_enable_waiting = 0;
			IME = 1;
			break;
		}
		case 0xC4: case 0xD4: case 0xCC: case 0xDC:
		{// CALL cc[y], (d16)
			cycles = 3;
			uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
			if ((this->*cc[op.y])()) {
				cycles += 3;
				write(--reg.SP.hl, (reg.PC >> 8));
				write(--reg.SP.hl, (reg.PC & 0x00FF));
				reg.PC = addr;
			}
			break;
		}
		case 0xC5: case 0xD5: case 0xE5: case 0xF5:
		{//PUSH rp2[p]
			cycles = 4;
			uint16_t data = *rp2[p];
			uint8_t h = (data >> 8);
			uint8_t l = (data & 0x00FF);
			write(--reg.SP.hl, h);
			write(--reg.SP.hl, l);
			break;
		}
		case 0xCD: {//CALL a16
			cycles = 6;
			uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
			write(--reg.SP.hl, (reg.PC >> 8));
			write(--reg.SP.hl, (reg.PC & 0x00FF));
			reg.PC = addr;
			break;
		}
		case 0xC6: case 0xD6: case 0xE6: case 0xF6: case 0xCE: case 0xDE: case 0xEE: case 0xFE:
		{// z = 6, alu[y],d8
			cycles = 2;
			uint8_t data = read(reg.PC++);
			(this->*alu[op.y])(data);
			break;
		}
		case 0xC7: case 0xD7: case 0xE7: case 0xF7: case 0xCF: case 0xDF: case 0xEF: case 0xFF:
		{
			//RST y*8
			cycles = 4;
			write(--reg.SP.hl, (reg.PC >> 8));
			write(--reg.SP.hl, (reg.PC & 0x00FF));
			reg.PC = (op.y << 3);
			break;
		}
		default:
			cycles = 1;
		}
		return cycles;
	}
	
	uint8_t CPU::exec_prefixed() {
		uint8_t cycles = 2;
		if (r[op.z] == nullptr) cycles = 4;
		switch (op.x)
		{
		case 0: {//rot[y] r[z]
			(this->*rot[op.y])(r[op.z]);
			break;
		}
		case 1: {//BIT y,r[z]
			uint8_t data;
			if (r[op.z] == nullptr) {// r[z] = (HL)
				cycles = 3;
				data = read(reg.HL.hl);
			}
			else {
				data = *r[op.z];
			}
			reg.AF.Z = ((data & (1 << (op.y))) == 0);
			reg.AF.N = 0;
			reg.AF.H = 1;
			break;
		}
		case 2: {//RES y,r[z]
			uint8_t data;
			if (r[op.z] == nullptr) {// r[z] = (HL)
				data = read(reg.HL.hl);
				data &= ~(1 << (op.y));
				write(reg.HL.hl, data);
			}
			else {
				*r[op.z] &= ~(1 << (op.y));
			}
			break;
		}
		case 3: {//SET y,r[z]
			uint8_t data;
			if (r[op.z] == nullptr) {// r[z] = (HL)
				data = read(reg.HL.hl);
				data |= (1 << (op.y));
				write(reg.HL.hl, data);
			}
			else {
				*r[op.z] |= (1 << (op.y));
			}
			break;
		}
		}
		return cycles;
	}

	//Arithmetic/logic operations
	void CPU::ADD(uint8_t data) {
		uint8_t res = reg.AF.h + data;

		reg.AF.N = 0;
		reg.AF.H = ((((reg.AF.h & 0x0F) + (data & 0x0F)) & 0x10) > 0);
		reg.AF.C = ((((uint16_t)reg.AF.h + (uint16_t)data) & 0x100) > 0);
		reg.AF.Z = (res == 0);
		reg.AF.h = res;
	}

	void CPU::ADC(uint8_t data) {
		uint8_t res = reg.AF.h + data + reg.AF.C;

		reg.AF.N = 0;
		reg.AF.H = ((((reg.AF.h & 0x0F) + (data & 0x0F) + reg.AF.C) & 0x10) > 0);
		reg.AF.C = ((((uint16_t)reg.AF.h + (uint16_t)data + reg.AF.C) & 0x100) > 0);
		reg.AF.Z = (res == 0);
		reg.AF.h = res;
	}

	void CPU::SUB(uint8_t data) {
		uint8_t res = reg.AF.h - data;

		reg.AF.N = 1;
		reg.AF.H = ((((reg.AF.h & 0x0F) - (data & 0x0F)) & 0x10) != 0);
		reg.AF.C = ((((uint16_t)reg.AF.h - (uint16_t)data) & 0x100) != 0);
		reg.AF.Z = (res == 0);
		reg.AF.h = res;
	}

	void CPU::SBC(uint8_t data) {
		uint8_t res = reg.AF.h - data - reg.AF.C;

		reg.AF.N = 1;
		reg.AF.H = ((((reg.AF.h & 0x0F) - (data & 0x0F) - reg.AF.C) & 0x10) != 0);
		reg.AF.C = ((((uint16_t)reg.AF.h - (uint16_t)data - reg.AF.C) & 0x100) != 0);
		reg.AF.Z = (res == 0);
		reg.AF.h = res;
	}
	void CPU::AND(uint8_t data) {
		reg.AF.h &= data;
		reg.AF.Z = (reg.AF.h == 0);
		reg.AF.N = 0;
		reg.AF.H = 1;
		reg.AF.C = 0;
	}
	void CPU::OR(uint8_t data) {
		reg.AF.h |= data;
		reg.AF.Z = (reg.AF.h == 0);
		reg.AF.N = 0;
		reg.AF.H = 0;
		reg.AF.C = 0;
	}
	void CPU::XOR(uint8_t data) {
		reg.AF.h ^= data;
		reg.AF.Z = (reg.AF.h == 0);
		reg.AF.N = 0;
		reg.AF.H = 0;
		reg.AF.C = 0;
	}
	void CPU::CP(uint8_t data) {
		reg.AF.Z = (reg.AF.h == data);
		reg.AF.N = 1;
		reg.AF.H = ((reg.AF.h & 0x0F) < (data & 0x0F));
		reg.AF.C = (reg.AF.h < data);
	}

	//Rotation/shift operations
	void CPU::RLC(uint8_t* data) {
		uint8_t target;
		if (data == nullptr) { // data = (HL)
			target = read(reg.HL.hl);
		}
		else {
			target = *data;
		}
		uint8_t CY = ((target & 0x80) >> 7);
		target = ((target << 1) | CY);
		reg.AF.C = CY;
		reg.AF.Z = (target == 0);
		reg.AF.N = 0;
		reg.AF.H = 0;
		if (data == nullptr) {
			write(reg.HL.hl, target);
		}
		else {
			*data = target;
		}
	}
	void CPU::RRC(uint8_t* data) {
		uint8_t target;
		if (data == nullptr) { // data = (HL)
			target = read(reg.HL.hl);
		}
		else {
			target = *data;
		}
		uint8_t CY = ((target & 0x01) << 7);
		target = ((target >> 1) | CY);
		reg.AF.C = (CY > 0);
		reg.AF.Z = (target == 0);
		reg.AF.N = 0;
		reg.AF.H = 0;
		if (data == nullptr) {
			write(reg.HL.hl, target);
		}
		else {
			*data = target;
		}
	}
	void CPU::RL(uint8_t* data) {
		uint8_t target;
		if (data == nullptr) { // data = (HL)
			target = read(reg.HL.hl);
		}
		else {
			target = *data;
		}
		uint8_t CY = reg.AF.C;
		reg.AF.C = ((target & 0x80) > 0);
		target = ((target << 1) | CY);
		reg.AF.Z = (target == 0);
		reg.AF.N = 0;
		reg.AF.H = 0;
		if (data == nullptr) {
			write(reg.HL.hl, target);
		}
		else {
			*data = target;
		}
	}
	void CPU::RR(uint8_t* data) {
		uint8_t target;
		if (data == nullptr) { // data = (HL)
			target = read(reg.HL.hl);
		}
		else {
			target = *data;
		}
		uint8_t CY = reg.AF.C;
		reg.AF.C = ((target & 0x01) > 0);
		target = ((target >> 1) | (CY << 7));
		reg.AF.Z = (target == 0);
		reg.AF.N = 0;
		reg.AF.H = 0;
		if (data == nullptr) {
			write(reg.HL.hl, target);
		}
		else {
			*data = target;
		}
	}
	void CPU::SLA(uint8_t* data) {
		uint8_t target;
		if (data == nullptr) { // data = (HL)
			target = read(reg.HL.hl);
		}
		else {
			target = *data;
		}
		reg.AF.C = ((target & 0x80) > 0);
		target <<= 1;
		reg.AF.Z = (target == 0);
		reg.AF.N = 0;
		reg.AF.H = 0;
		if (data == nullptr) {
			write(reg.HL.hl, target);
		}
		else {
			*data = target;
		}
	}
	void CPU::SRA(uint8_t* data) {
		uint8_t target;
		if (data == nullptr) { // data = (HL)
			target = read(reg.HL.hl);
		}
		else {
			target = *data;
		}
		reg.AF.C = ((target & 0x01) > 0);
		target = ((target >> 1) | (target & 0x80));
		reg.AF.Z = (target == 0);
		reg.AF.N = 0;
		reg.AF.H = 0;
		if (data == nullptr) {
			write(reg.HL.hl, target);
		}
		else {
			*data = target;
		}
	}
	void CPU::SRL(uint8_t* data) {
		uint8_t target;
		if (data == nullptr) { // data = (HL)
			target = read(reg.HL.hl);
		}
		else {
			target = *data;
		}
		reg.AF.C = ((target & 0x01) > 0);
		target >>= 1;
		reg.AF.Z = (target == 0);
		reg.AF.N = 0;
		reg.AF.H = 0;
		if (data == nullptr) {
			write(reg.HL.hl, target);
		}
		else {
			*data = target;
		}
	}
	void CPU::SWAP(uint8_t* data) {
		uint8_t target;
		if (data == nullptr) { // data = (HL)
			target = read(reg.HL.hl);
		}
		else {
			target = *data;
		}
		target = ((target << 4) | (target >> 4));
		reg.AF.Z = (target == 0);
		reg.AF.C = 0;
		reg.AF.H = 0;
		reg.AF.N = 0;
		if (data == nullptr) {
			write(reg.HL.hl, target);
		}
		else {
			*data = target;
		}
	}
	//flag checking
	bool CPU::C() {
		return reg.AF.C;
	}
	bool CPU::NC() {
		return !reg.AF.C;
	}
	bool CPU::Z() {
		return reg.AF.Z;
	}
	bool CPU::NZ() {
		return !reg.AF.Z;
	}


}