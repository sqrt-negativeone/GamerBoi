#include "CPU2.h"
#include "Bus.h"

void CPU2::connectBus(Bus* b) {
	this->bus = b;
	reset();
}

void CPU2::reset() {
	reg.AF.hl = 0x01B0;
	reg.BC.hl = 0x0013;
	reg.DE.hl = 0x00D8;
	reg.HL.hl = 0x014D;
	reg.SP.hl = 0xFFFE;
	reg.PC = 0x0100;
	IE = 0;
	IME = 0;
	bus->write(0xFFFF, 0x00);
}

uint8_t CPU2::read_IE_flag() {
	write_IE_flag(IE);
	return IE;
}
void CPU2::write_IE_flag(uint8_t data) {
	IE = (data & 0x1F);
}

uint8_t CPU2::read(uint16_t addr) {
	return bus->read(addr);
}

void CPU2::write(uint16_t addr, uint8_t data) {
	bus->write(addr, data);
}

void CPU2::clock() {
	if (cycle == 0) {
		if (is_halt) return;
		if (halt_bug) {
			op.opcode = read(reg.PC);
			halt_bug = false;
		}
		else op.opcode = read(reg.PC++);
 		cycle = exec_opcode();
	}
	clock_count++;
	cycle--;
}


void CPU2::handle_intr() {
	uint8_t IE = read(0xFFFF); // Get Interrupt Enable flag
	uint8_t IF = read(0xFF0F); // Get Interrupt Flag
	/*if (ime_enable_waiting >= 0) {
		ime_enable_waiting++;
		if (ime_enable_waiting == 2) {
			IME = 1;
			ime_enable_waiting = -1;
		}
	}
	if (ime_disable_waiting >= 0) {
		ime_disable_waiting++;
		if (ime_disable_waiting == 2) {
			IME = 0;
			ime_disable_waiting = -1;
		}
	}*/

	if (IME) {

		if (IE & IF & 0x1F)
		{
			is_halt = 0;

			//following the way Z80 execute interruption.. (yet to be tested)
			cycle += 5;
		}
		else return;

		if ((IE & IF & 0x01)) { // V-Blank Interrupt 
			IF &= ~(0x01); // Reset the flag
			IME = 0; // Clear IME flag
			write(--reg.SP.hl, reg.PC >> 8); //push the PC to stack
			write(--reg.SP.hl, (reg.PC & 0x00FF));
			reg.PC = 0x0040; //jump to interrupt location
			write(0xFF0F, IF);
		}

		else if ((IE & IF & 0x02)) { // LCD STAT Interrupt 
			IF &= ~(0x02);
			IME = 0;
			write(--reg.SP.hl, reg.PC >> 8);
			write(--reg.SP.hl, (reg.PC & 0x00FF));
			reg.PC = 0x0048;
			write(0xFF0F, IF);
		}
		else if ((IE & IF & 0x04)) { // Timer Interrupt 
			IF &= ~(0x04);
			IME = 0;
			write(--reg.SP.hl, reg.PC >> 8);
			write(--reg.SP.hl, (reg.PC & 0x00FF));
			reg.PC = 0x0050;
			write(0xFF0F, IF);
		}
		else if ((IE & IF & 0x08)) { // Serial Interrupt 
			IF &= ~(0x08);
			IME = 0;
			write(--reg.SP.hl, reg.PC >> 8);
			write(--reg.SP.hl, (reg.PC & 0x00FF));
			reg.PC = 0x0058;
			write(0xFF0F, IF);
		}
		else if ((IE & IF & 0x10)) { // Joypad Interrupt 
			IF &= ~(0x10);
			IME = 0;
			write(--reg.SP.hl, reg.PC >> 8);
			write(--reg.SP.hl, (reg.PC & 0x00FF));
			reg.PC = 0x0060;
			write(0xFF0F, IF);
		}
	}
	else {
		if (is_halt && (IE & IF & 0x1F)) {
			is_halt = false;
		}
	}
}


uint8_t CPU2::exec_opcode() {
	uint8_t cycles = 0;
	uint8_t p = (op.y >> 1);
	uint8_t q = (op.y & 1);
	switch (op.x)
	{
	case 0: {//x=0
		switch (op.z)
		{
		case 0: {
			switch (op.y)
			{
			case 0: cycles = 1; break;//NOP
			case 1: { //LD (a16), SP
				cycles = 5;
				uint16_t l = read(reg.PC++);
				uint16_t h = read(reg.PC++);
				uint16_t addr = ((h << 8) | l);
				write(addr++, reg.SP.l);
				write(addr, reg.SP.h);
				break;
			}
			case 2: {//STOP
				cycles = 1;
				is_stop = 1;
				break;
			}
			case 3: {//JR e
				cycles = 3;
				int8_t data = (int8_t)read(reg.PC++);
				reg.PC += data;
				break;
			}
			default://JR cc,d
				cycles = 2;
				int8_t data = (int8_t)read(reg.PC++);
				if ((this->*cc[op.y - 4])()) {
					cycles++;
					reg.PC += data;
				}
				break;
			}
			break;
		}
		case 1: {
			if (q == 0) {// LD rp[p],nn
				cycles = 3;
				uint8_t l = read(reg.PC++);
				uint16_t h = read(reg.PC++);
				*rp[p] = ((h << 8) | l);
			}
			else {// ADD HL,rp[p]
				cycles = 2;

				uint16_t tmp1 = *rp[p];
				uint16_t tmp2 = reg.HL.hl;
				uint16_t ans = tmp1 + tmp2;

				reg.HL.hl = ans;

				//set flags
				reg.AF.N = 0;
				reg.AF.H = ((((tmp1 & 0x0FFF) + (tmp2 & 0x0FFF)) & 0x1000) > 0);
				reg.AF.C = ((((tmp1 & 0x0FFFF) + (tmp2 & 0x0FFFF)) & 0x10000) > 0);
			}
			break;
		}
		case 2: {
			cycles = 2;
			if (q == 0) {
				switch (p)
				{
				case 0: {// LD (BC),A
					write(reg.BC.hl, reg.AF.h);
					break;
				}
				case 1: {// LD (DE),A
					write(reg.DE.hl, reg.AF.h);
					break;

				}
				case 2: {//LD (HL+),A
					write(reg.HL.hl++, reg.AF.h);
					break;

				}
				case 3: {//LD (HL-),A
					write(reg.HL.hl--, reg.AF.h);
					break;
				}
				}
			}
			else {
				switch (p)
				{
				case 0: {//LD A,(BC)
					reg.AF.h = read(reg.BC.hl);
					break;
				}
				case 1: {//LD A,(DE)
					reg.AF.h = read(reg.DE.hl);
					break;
				}
				case 2: {//LD A,(HL+)
					reg.AF.h = read(reg.HL.hl++);
					break;
				}
				case 3: {//LD A,(HL-)
					reg.AF.h = read(reg.HL.hl--);
					break;
				}
				}
			}
			break;
		}
		case 3: {//z=3
			cycles = 2;
			if (q == 0) {// INC rp[p]
				(*rp[p])++;
			}
			else {
				(*rp[p])--;
			}
			break;
		}
		case 4: {//z=4
			if (r[op.y] == nullptr) {// INC (HL)
				cycles = 3;
				uint8_t data = read(reg.HL.hl);
				reg.AF.H = ((((data & 0x0F) + 1) & 0x10) > 0);
				data++;
				reg.AF.Z = (data == 0);
				reg.AF.N = 0;
				write(reg.HL.hl, data);
			}
			else {// INC r[y]
				cycles = 1;
				reg.AF.H = (((((*r[op.y]) & 0x0F) + 1) & 0x10) > 0);
				(*r[op.y])++;
				reg.AF.Z = (*r[op.y] == 0);
				reg.AF.N = 0;
			}
			break;
		}
		case 5: {//z=5
			if (r[op.y] == nullptr) {// DEC (HL)
				cycles = 3;
				uint8_t data = read(reg.HL.hl);
				reg.AF.H = ((((data & 0x0F) - 1) & 0x10) != 0);
				data--;
				reg.AF.Z = (data == 0);
				reg.AF.N = 1;
				write(reg.HL.hl, data);
			}
			else {// DEC r[y]
				cycles = 1;
				reg.AF.H = (((((*r[op.y]) & 0x0F) - 1) & 0x10) != 0);
				(*r[op.y])--;
				reg.AF.Z = (*r[op.y] == 0);
				reg.AF.N = 1;
			}
			break;
		}
		case 6: {//z=6
			if (r[op.y] == nullptr) {// LD (HL),d8
				cycles = 3;
				uint8_t data = read(reg.PC++);
				write(reg.HL.hl, data);
			}
			else {// LD r[y],d8
				cycles = 2;
				*r[op.y] = read(reg.PC++);
			}
			break;
		}
		case 7: {// z=7
			cycles = 1;
			switch (op.y)
			{
			case 0: {// RLCA
				uint8_t CY = ((reg.AF.h & 0x80) >> 7);
				reg.AF.C = CY;
				reg.AF.Z = 0;
				reg.AF.N = 0;
				reg.AF.H = 0;
				reg.AF.h = ((reg.AF.h << 1) | CY);
				break;
			}
			case 1: {//RRCA
				uint8_t CY = (reg.AF.h & 0x01);

				reg.AF.C = CY;
				reg.AF.Z = 0;
				reg.AF.N = 0;
				reg.AF.H = 0;

				reg.AF.h = ((reg.AF.h >> 1) | (CY << 7));
				break;
			}
			case 2: {//RLA
				uint8_t CY = reg.AF.C;
				reg.AF.C = ((reg.AF.h & 0x80) > 0);
				reg.AF.Z = 0;
				reg.AF.N = 0;
				reg.AF.H = 0;

				reg.AF.h = ((reg.AF.h << 1) | CY);
				break;
			}
			case 3: {//RRA
				uint8_t CY = reg.AF.C;

				reg.AF.C = (reg.AF.h & 1);
				reg.AF.Z = 0;
				reg.AF.N = 0;
				reg.AF.H = 0;

				reg.AF.h = ((reg.AF.h >> 1) | (CY << 7));
				break;
			}
			case 4: {//DAA
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
			case 5: {//CPL
				reg.AF.h = ~reg.AF.h;
				reg.AF.N = 1;
				reg.AF.H = 1;
				break;
			}
			case 6: {//SCF
				reg.AF.C = 1;
				reg.AF.H = 0;
				reg.AF.N = 0;
				break;
			}
			case 7: {//CCF
				reg.AF.C ^= 1;
				reg.AF.H = 0;
				reg.AF.N = 0;
				break;
			}
			}
			break;
		}
		}
		break;
	}
	case 1: {//x=1
		if (op.y == 6 && op.z == 6) {//HALT
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
		}
		else {//LD r[y],r[z]
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
		}
		break;
	}
	case 2: {//x=2, alu[y] r[z]
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
	case 3: {//x=3
		switch (op.z)
		{
		case 0: {// z = 0
			switch (op.y)
			{
			case 0:
			case 1:
			case 2:
			case 3: {// RET cc[y]
				cycles = 2;
				if ((this->*cc[op.y])()) {
					cycles = 5;
					uint8_t l = read(reg.SP.hl++);
					uint16_t h = read(reg.SP.hl++);
					reg.PC = ((h << 8) | l);
				}
				break;
			}
			case 4: {//LD (0xFF00+d8),A
				cycles = 3;
				uint8_t data = read(reg.PC++);
				uint16_t addr = 0xFF00 + data;
				write(addr, reg.AF.h);
				break;
			}
			case 5: {//ADD SP,e
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
			case 6: {//LD A,(0xFF00+d8)
				cycles = 3;
				uint8_t data = read(reg.PC++);
				uint16_t addr = 0xFF00 + data;
				reg.AF.h = read(addr);
				break;
			}
			case 7: {// LD HL, (SP+d8)
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
			}
			break;
		}
		case 1: { //z = 1
			if (q == 0) {// POP rp2[p]
				cycles = 3;
				uint8_t l = read(reg.SP.hl++);
				uint16_t h = read(reg.SP.hl++);
				*rp2[p] = ((h << 8) | l);
				if (p == 3) {
					reg.AF.unused = 0;
				}
			}
			else {
				switch (p)
				{
				case 0:
				case 1: {// RET/ RETI
					cycles = 4;
					uint8_t l = read(reg.SP.hl++);
					uint16_t h = read(reg.SP.hl++);
					reg.PC = ((h << 8) | l);
					if (p) IME = 1;
					break;
				}
				case 2: {//JP HL
					cycles = 1;
					reg.PC = reg.HL.hl;
					break;
				}
				case 3: {//LD SP,HL
					cycles = 2;
					reg.SP.hl = reg.HL.hl;
					break;
				}
				}
			}
			break;
		}
		case 2: {//z = 2
			switch (op.y)
			{
			case 0:
			case 1:
			case 2:
			case 3: {// JP cc[y], nn
				cycles = 3;
				uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
				if ((this->*cc[op.y])()) {
					cycles++;
					reg.PC = addr;
				}
				break;
			}
			case 4: {//LD (0xFF00+C),A
				cycles = 2;
				uint16_t addr = 0xFF00 + reg.BC.l;
				write(addr, reg.AF.h);
				break;
			}
			case 5: {//LD (a16),A
				cycles = 4;
				uint8_t l = read(reg.PC++);
				uint16_t h = read(reg.PC++);
				uint16_t addr = ((h << 8) | l);
				write(addr, reg.AF.h);
				break;
			}
			case 6: {//LD A,(0xFF00+C)
				cycles = 2;
				reg.AF.h = read(0xFF00 + reg.BC.l);
				break;
			}
			case 7: {//LD A,(a16)
				cycles = 4;
				uint8_t l = read(reg.PC++);
				uint16_t h = read(reg.PC++);
				uint16_t addr = ((h << 8) | l);
				reg.AF.h = read(addr);
				break;
			}
			}
			break;
		}
		case 3: {//z = 3
			switch (op.y)
			{
			case 0: {// JP a16
				cycles = 4;
				uint8_t l = read(reg.PC++);
				uint16_t h = read(reg.PC++);
				reg.PC = (l | (h << 8));
				break;
			}
			case 1: {//CB prefix
				op.opcode = read(reg.PC++);
				cycles = exec_prefixed();
				break;
			}
			case 6: {//DI
				cycles = 1;
				//ime_disable_waiting = 0;
				IME = 0;
				break;
			}
			case 7: {//EI
				cycles = 1;
				//ime_enable_waiting = 0;
				IME = 1;
				break;
			}
			default: {//UNDEFINED
				cycles = 1;
			}
			}
			break;
		}
		case 4: {// z = 4
			if (op.y < 4) {// CALL cc[y], (d16)
				cycles = 3;
				uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
				if ((this->*cc[op.y])()) {
					cycles += 3;
					write(--reg.SP.hl, (reg.PC >> 8));
					write(--reg.SP.hl, (reg.PC & 0x00FF));
					reg.PC = addr;
				}
			}
			else {//UNDEFINED
				cycles = 1;
			}
			break;
		}
		case 5: {// z = 5
			if (q == 0) {//PUSH rp2[p]
				cycles = 4;
				uint16_t data = *rp2[p];
				uint8_t h = (data >> 8);
				uint8_t l = (data & 0x00FF);
				write(--reg.SP.hl, h);
				write(--reg.SP.hl, l);
			}
			else {
				if (p == 0) {//CALL a16
					cycles = 6;
					uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
					write(--reg.SP.hl, (reg.PC >> 8));
					write(--reg.SP.hl, (reg.PC & 0x00FF));
					reg.PC = addr;
				}
				else {//UNDEFINED
					cycles = 1;
				}
			}
			break;
		}
		case 6: {// z = 6, alu[y],d8
			cycles = 2;
			uint8_t data = read(reg.PC++);
			(this->*alu[op.y])(data);
			break;
		}
		case 7: {// z = 7
			//RST y*8
			cycles = 4;
			write(--reg.SP.hl, (reg.PC >> 8));
			write(--reg.SP.hl, (reg.PC & 0x00FF));
			reg.PC = (op.y << 3);
			break;
		}
		}
		break;
	}
	}
	return cycles;
}

uint8_t CPU2::exec_prefixed() {
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
void CPU2::ADD(uint8_t data) {
	uint8_t res = reg.AF.h + data;

	reg.AF.N = 0;
	reg.AF.H = ((((reg.AF.h & 0x0F) + (data & 0x0F)) & 0x10) > 0);
	reg.AF.C = ((((uint16_t)reg.AF.h + (uint16_t)data) & 0x100) > 0);
	reg.AF.Z = (res == 0);
	reg.AF.h = res;
}

void CPU2::ADC(uint8_t data) {
	uint8_t res = reg.AF.h + data + reg.AF.C;

	reg.AF.N = 0;
	reg.AF.H = ((((reg.AF.h & 0x0F) + (data & 0x0F) + reg.AF.C) & 0x10) > 0);
	reg.AF.C = ((((uint16_t)reg.AF.h + (uint16_t)data + reg.AF.C) & 0x100) > 0);
	reg.AF.Z = (res == 0);
	reg.AF.h = res;
}

void CPU2::SUB(uint8_t data) {
	uint8_t res = reg.AF.h - data;

	reg.AF.N = 1;
	reg.AF.H = ((((reg.AF.h & 0x0F) - (data & 0x0F)) & 0x10) != 0);
	reg.AF.C = ((((uint16_t)reg.AF.h - (uint16_t)data) & 0x100) != 0);
	reg.AF.Z = (res == 0);
	reg.AF.h = res;
}

void CPU2::SBC(uint8_t data) {
	uint8_t res = reg.AF.h - data - reg.AF.C;

	reg.AF.N = 1;
	reg.AF.H = ((((reg.AF.h & 0x0F) - (data & 0x0F) - reg.AF.C) & 0x10) != 0);
	reg.AF.C = ((((uint16_t)reg.AF.h - (uint16_t)data - reg.AF.C) & 0x100) != 0);
	reg.AF.Z = (res == 0);
	reg.AF.h = res;
}
void CPU2::AND(uint8_t data) {
	reg.AF.h &= data;
	reg.AF.Z = (reg.AF.h == 0);
	reg.AF.N = 0;
	reg.AF.H = 1;
	reg.AF.C = 0;
}
void CPU2::OR(uint8_t data) {
	reg.AF.h |= data;
	reg.AF.Z = (reg.AF.h == 0);
	reg.AF.N = 0;
	reg.AF.H = 0;
	reg.AF.C = 0;
}
void CPU2::XOR(uint8_t data) {
	reg.AF.h ^= data;
	reg.AF.Z = (reg.AF.h == 0);
	reg.AF.N = 0;
	reg.AF.H = 0;
	reg.AF.C = 0;
}
void CPU2::CP(uint8_t data) {
	reg.AF.Z = (reg.AF.h == data);
	reg.AF.N = 1;
	reg.AF.H = ((reg.AF.h & 0x0F) < (data & 0x0F));
	reg.AF.C = (reg.AF.h < data);
}

//Rotation/shift operations
void CPU2::RLC(uint8_t* data) {
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
void CPU2::RRC(uint8_t* data) {
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
void CPU2::RL(uint8_t* data) {
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
void CPU2::RR(uint8_t* data) {
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
void CPU2::SLA(uint8_t* data) {
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
void CPU2::SRA(uint8_t* data) {
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
void CPU2::SRL(uint8_t* data) {
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
void CPU2::SWAP(uint8_t* data) {
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
bool CPU2::C() {
	return reg.AF.C;
}
bool CPU2::NC() {
	return !reg.AF.C;
}
bool CPU2::Z() {
	return reg.AF.Z;
}
bool CPU2::NZ() {
	return !reg.AF.Z;
}