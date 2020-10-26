#include "CPU.h"

//In seperated file for easy bug fxing X)
uint8_t CPU::exec_opcode(uint8_t opcode) {
	uint8_t cycles = 0;
	switch (opcode) {
	case 0x00: { //NOP
		cycles = 1;
		break;
	}
	case 0x01: { //LD BC,d16
		cycles = 3;
		uint8_t l = read(reg.PC++);
		uint8_t h = read(reg.PC++);
		reg.C = l;
		reg.B = h;
		break;
	}
	case 0x02: { //LD (BC),A
		cycles = 2;
		uint16_t addr = ((int16_t)reg.B << 8) | (uint16_t)reg.C;
		write(addr, reg.A);
		break;
	}
	case 0x03: { //INC BC
		cycles = 2;
		uint16_t tmp = ((int16_t)reg.B << 8) | (uint16_t)reg.C;
		tmp++;
		reg.B = (tmp >> 8);
		reg.C = (tmp & 0x00FF);
		break;
	}
	case 0x04: { //INC B
		cycles = 1;
		setFlag(H, ((reg.B & 0x0F) + 1) & 0x10);
		reg.B++;
		setFlag(Z, reg.B == 0);
		setFlag(N, 0);
		break;
	}
	case 0x05: { //DEC B
		cycles = 1;
		setFlag(H, ((reg.B & 0x0F) - 1) & 0x10);
		reg.B--;
		setFlag(Z, reg.B == 0);
		setFlag(N, 1);
		break;
	}
	case 0x06: { //LD B,d8
		cycles = 2;
		uint8_t data = read(reg.PC++);
		reg.B = data;
		break;
	}
	case 0x07: { //RLCA
		cycles = 1;
		uint8_t CY = ((reg.A & 0x80) >> 7);

		setFlag(C, CY);
		setFlag(Z, 0);
		setFlag(N, 0);
		setFlag(H, 0);

		reg.A = ((reg.A << 1) | CY);
		break;
	}
	case 0x08: { //LD (a16),SP
		cycles = 5;
		uint16_t l = read(reg.PC++);
		uint16_t h = read(reg.PC++);
		uint16_t addr = (h << 8) | l;
		write(addr++, reg.SP & 0x00FF);
		write(addr, (reg.SP >> 8));
		break;
	}
	case 0x09: { //ADD HL,BC
		cycles = 2;

		uint16_t tmp1 = ((uint16_t)reg.B << 8) | (uint16_t)reg.C;
		uint16_t tmp2 = ((uint16_t)reg.H << 8) | (uint16_t)reg.L;
		uint16_t ans = tmp1 + tmp2;

		reg.H = (ans >> 8);
		reg.L = (ans & 0x00FF);

		setFlag(N, 0);
		setFlag(H, ((tmp1 & 0x00FF) + (tmp2 & 0x00FF)) & 0x0100);
		setFlag(C, ((uint32_t)tmp1 + ((uint32_t)tmp2) & 0x10000));
		break;
	}
	case 0x0A: { // LD A,(BC)
		cycles = 2;
		uint16_t addr = ((uint16_t)reg.B << 8) | (uint16_t)reg.C;
		reg.A = read(addr);
		break;
	}
	case 0x0B: { //DEC BC
		cycles = 2;
		uint16_t tmp = ((int16_t)reg.B << 8) | (uint16_t)reg.C;
		tmp--;
		reg.B = (tmp >> 8);
		reg.C = (tmp & 0x00FF);
		break;
	}
	case 0x0C: {//INC C
		cycles = 1;
		setFlag(H, (((reg.C & 0x0F) + 1) & 0x10));
		reg.C++;
		setFlag(Z, reg.C == 0);
		setFlag(N, 0);
		break;
	}
	case 0x0D: {//DEC C
		cycles = 1;
		setFlag(H, (((reg.C & 0x0F) - 1) & 0x10));
		reg.C--;
		setFlag(Z, reg.C == 0);
		setFlag(N, 1);
		break;
	}
	case 0x0E: {//LD C,d8
		cycles = 2;
		uint8_t data = read(reg.PC++);
		reg.C = data;
		break;
	}
	case 0x0F: { //RRCA
		cycles = 1;
		uint8_t CY = (reg.A & 0x01);

		setFlag(C, CY);
		setFlag(Z, 0);
		setFlag(N, 0);
		setFlag(H, 0);

		reg.A = ((reg.A >> 1) | (CY << 7));
		break;
	}
	case 0x10: {//STOP
		cycles = 1;
		is_stop = 1;
		break;
	}
	case 0x11: { //LD DE,d16
		cycles = 3;
		uint8_t l = read(reg.PC++);
		uint8_t h = read(reg.PC++);
		reg.E = l;
		reg.D = h;
		break;
	}
	case 0x12: { //LD (DE),A
		cycles = 2;
		uint16_t addr = ((int16_t)reg.D << 8) | (uint16_t)reg.E;
		write(addr, reg.A);
		break;
	}
	case 0x13: {//INC DE
		cycles = 2;
		uint16_t tmp = ((int16_t)reg.D << 8) | (uint16_t)reg.E;
		tmp++;
		reg.D = (tmp >> 8);
		reg.E = (tmp & 0x00FF);
		break;
	}
	case 0x14: { //INC D
		cycles = 1;
		setFlag(H, (((reg.D & 0x0F) + 1) & 0x10));
		reg.D++;
		setFlag(Z, reg.D == 0);
		setFlag(N, 0);
		break;
	}
	case 0x15: {// DEC D
		cycles = 1;
		setFlag(H, ((reg.D & 0x0F) - 1) & 0x10);
		reg.D--;
		setFlag(Z, reg.D == 0);
		setFlag(N, 1);
		break;
	}
	case 0x16: {// LD D,d8
		cycles = 2;
		uint8_t data = read(reg.PC++);
		reg.D = data;
		break;
	}
	case 0x17: {// RLA
		cycles = 1;
		uint8_t CY = getFlag(C);

		setFlag(C, (reg.A & 0x80));
		setFlag(Z, 0);
		setFlag(N, 0);
		setFlag(H, 0);

		reg.A = ((reg.A << 1) | CY);
		break;
	}
	case 0x18: {// JR r8
		cycles = 3;
		int8_t data = (int8_t) read(reg.PC++);
		reg.PC += data;
		break;
	}
	case 0x19: {// ADD HL, DE
		cycles = 2;

		uint16_t tmp1 = ((uint16_t)reg.D << 8) | (uint16_t)reg.E;
		uint16_t tmp2 = ((uint16_t)reg.H << 8) | (uint16_t)reg.L;
		uint16_t ans = tmp1 + tmp2;

		reg.H = (ans >> 8);
		reg.L = (ans & 0x00FF);

		setFlag(N, 0);
		setFlag(H, ((tmp1 & 0x00FF) + (tmp2 & 0x00FF)) & 0x0100);
		setFlag(C, ((uint32_t)tmp1 + (uint32_t)tmp2) & 0x10000);
		break;
	}
	case 0x1A: {// LD A,(DE)
		cycles = 2;
		uint16_t addr = ((uint16_t)reg.D << 8) | (uint16_t)reg.E;
		reg.A = read(addr);
		break;
	}
	case 0x1B: {//DEC DE
		cycles = 2;
		uint16_t tmp = ((int16_t)reg.D << 8) | (uint16_t)reg.E;
		tmp--;
		reg.B = (tmp >> 8);
		reg.C = (tmp & 0x00FF);
		break;
	}
	case 0x1C: {// INC E 
		cycles = 1;
		setFlag(H, ((reg.E & 0x0F) + 1) & 0x10);
		reg.E++;
		setFlag(Z, reg.E == 0);
		setFlag(N, 0);
		break;
	}
	case 0x1D: {// DEC E
		cycles = 1;
		setFlag(H, ((reg.E & 0x0F) - 1) & 0x10);
		reg.E--;
		setFlag(Z, reg.E == 0);
		setFlag(N, 1);
		break;
	}
	case 0x1E: {// LD E,d8
		cycles = 2;
		uint8_t data = read(reg.PC++);
		reg.E = data;
		break;
	}
	case 0x1F: { //RRA
		cycles = 1;
		uint8_t CY = getFlag(C);

		setFlag(C, reg.A & 0x01);
		setFlag(Z, 0);
		setFlag(N, 0);
		setFlag(H, 0);

		reg.A = ((reg.A >> 1) | (CY << 7));

		break;
	}
	case 0x20: {// JR NZ,r8
		cycles = 2;
		int8_t data =(int8_t) read(reg.PC++);
		if (!getFlag(Z)) {
			cycles++;
			reg.PC += data;
		}
		break;
	}
	case 0x21: {// LD HL, d16
		cycles = 3;
		uint8_t l = read(reg.PC++);
		uint8_t h = read(reg.PC++);
		reg.L = l;
		reg.H = h;
		break;
	}
	case 0x22: {// LD (HL+), A
		cycles = 2;
		uint16_t addr = ((int16_t)reg.H << 8) | (uint16_t)reg.L;
		write(addr++, reg.A);
		reg.H = (addr >> 8);
		reg.L = (addr & 0x00FF);
		break;
	}
	case 0x23: {// INC HL
		cycles = 2;
		uint16_t tmp = ((int16_t)reg.H << 8) | (uint16_t)reg.L;
		tmp++;
		reg.H = (tmp >> 8);
		reg.L = (tmp & 0x00FF);
		break;
	}
	case 0x24: {//INC H
		cycles = 1;
		setFlag(H, ((reg.H & 0x0F) + 1) & 0x10);
		reg.H++;
		setFlag(Z, reg.H == 0);
		setFlag(N, 0);
		break;
	}
	case 0x25: {//DEC H
		cycles = 1;
		setFlag(H, ((reg.H & 0x0F) - 1) & 0x10);
		reg.H--;
		setFlag(Z, reg.H == 0);
		setFlag(N, 1);
		break;
	}
	case 0x26: {//LD H,d8
		cycles = 2;
		uint8_t data = read(reg.PC++);
		reg.H = data;
		break;
	}
	case 0x27: {// DAA
		cycles = 1;
		bool CY = 0;
		if (getFlag(N)) {
			if (getFlag(C)) reg.A -= 0x60;
			if (getFlag(H)) reg.A -= 0x06;
		}
		else {
			if (getFlag(C) || (reg.A > 0x99)) { reg.A += 0x60; CY = 1; }
			if (getFlag(H) || (reg.A > 0x09)) reg.A += 0x06;
		}
		setFlag(C, CY);
		setFlag(Z, reg.A == 0);
		setFlag(H, 0);
		break;
	}
	case 0x28: {// JR Z,r8
		cycles = 2;
		int8_t data = read(reg.PC++);
		if (getFlag(Z)) {
			cycles++;
			reg.PC += data;
		}
		break;
	}
	case 0x29: {// ADD HL,HL
		cycles = 2;

		uint16_t tmp1 = ((uint16_t)reg.H << 8) | (uint16_t)reg.L;
		uint16_t ans = tmp1 + tmp1;

		reg.H = (ans >> 8);
		reg.L = (ans & 0x00FF);

		setFlag(N, 0);
		setFlag(H, ((tmp1 & 0x00FF) + (tmp1 & 0x00FF)) & 0x0100);
		setFlag(C, ((uint32_t)tmp1 + (uint32_t)tmp1) & 0x10000);
		break;
	}
	case 0x2A: {// LD A,(HL+)
		cycles = 2;
		uint16_t addr = ((uint16_t)reg.H << 8) | (uint16_t)reg.L;
		reg.A = read(addr);
		addr++;
		reg.H = (addr >> 8);
		reg.L = (addr & 0x00FF);
		break;
	}
	case 0x2B: {//DEC HL
		cycles = 2;
		uint16_t tmp = ((int16_t)reg.H << 8) | (uint16_t)reg.L;
		tmp--;
		reg.H = (tmp >> 8);
		reg.L = (tmp & 0x00FF);
		break;
	}
	case 0x2C: {//INC L
		cycles = 1;
		setFlag(H, ((reg.L & 0x0F) + 1) & 0x10);
		reg.L++;
		setFlag(Z, reg.L == 0);
		setFlag(N, 0);
		break;
	}
	case 0x2D: {//DEC L
		cycles = 1;
		setFlag(H, ((reg.L & 0x0F) - 1) & 0x10);
		reg.L--;
		setFlag(Z, reg.L == 0);
		setFlag(N, 1);
		break;
	}
	case 0x2E: {// LD L,d8
		cycles = 2;
		uint8_t data = read(reg.PC++);
		reg.L = data;
		break;
	}
	case 0x2F: {// CPL
		cycles = 1;
		reg.A = ~reg.A;
		setFlag(N, 1);
		setFlag(H, 1);
		break;
	}
	case 0x30: {// JR NC, r8
		cycles = 2;
		int8_t data = (int8_t)read(reg.PC++);
		if (!getFlag(C)) {
			cycles++;
			reg.PC += data;
		}
		break;
	}
	case 0x31: {// LD SP,d16
		cycles = 3;
		uint8_t l = read(reg.PC++);
		uint16_t h = read(reg.PC++);
		reg.SP = (h << 8) | l;
		break;
	}
	case 0x32: {// LD (HL-),A
		cycles = 2;
		uint16_t addr = ((int16_t)reg.H << 8) | (uint16_t)reg.L;
		write(addr--, reg.A);
		reg.H = (addr >> 8);
		reg.L = (addr & 0x00FF);
		break;
	}
	case 0x33: {// INC SP
		cycles = 2;
		reg.SP++;
		break;
	}
	case 0x34: {// INC (HL)
		cycles = 3;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		setFlag(H, ((data & 0x0F) + 1) & 0x10);
		data++;
		setFlag(Z, data == 0);
		write(addr, data);
		setFlag(N, 0);
		break;
	}
	case 0x35: {// DEC (HL)
		cycles = 3;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		setFlag(H, ((data & 0x0F) - 1) & 0x10);
		data--;
		setFlag(Z, data == 0);
		write(addr, data);
		setFlag(N, 1);
		break;
	}
	case 0x36: {// LD (HL),d8
		cycles = 3;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(reg.PC++);
		write(addr, data);
		break;
	}
	case 0x37: {// SCF
		cycles = 1;
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 1);
		break;
	}
	case 0x38: {// JR C,r8
		cycles = 2;
		int8_t data = (int8_t)read(reg.PC++);
		if (getFlag(C)) {
			cycles++;
			reg.PC += data;
		}
		break;
	}
	case 0x39: {// ADD HL, SP
		cycles = 2;

		uint16_t tmp1 = ((uint16_t)reg.H << 8) | (uint16_t)reg.L;
		uint16_t ans = tmp1 + reg.SP;

		reg.H = (ans >> 8);
		reg.L = (ans & 0x00FF);

		setFlag(N, 0);
		setFlag(H, ((tmp1 & 0x00FF) + (reg.SP & 0x00FF)) & 0x0100);
		setFlag(C, ((uint32_t)tmp1 + (uint32_t)reg.SP) & 0x10000);
		break;
	}
	case 0x3A: {// LD A, (HL-)
		cycles = 2;
		uint16_t addr = ((uint16_t)reg.H << 8) | (uint16_t)reg.L;
		reg.A = read(addr);
		addr--;
		reg.H = (addr >> 8);
		reg.L = (addr & 0x00FF);
		break;
	}
	case 0x3B: {// DEC SP
		cycles = 2;
		reg.SP--;
		break;
	}
	case 0x3C: {// INC A
		cycles = 1;
		setFlag(H, ((reg.A & 0x0F) + 1) & 0x10);
		reg.A++;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		break;
	}
	case 0x3D: {// DEC A
		cycles = 1;
		setFlag(H, ((reg.A & 0x0F) - 1) & 0x10);
		reg.A--;
		setFlag(Z, reg.A == 0);
		setFlag(N, 1);
		break;
	}
	case 0x3E: {// LD A,d8
		cycles = 2;
		uint8_t data = read(reg.PC++);
		reg.A = data;
		break;
	}
	case 0x3F: {// CCF
		cycles = 1;
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, ~getFlag(C));
		break;
	}
	case 0x40: {// LD B,B
		cycles = 1;
		break;
	}
	case 0x41: {// LD B,C
		cycles = 1;
		reg.B = reg.C;
		break;
	}
	case 0x42: {// LD B,D
		cycles = 1;
		reg.B = reg.D;
		break;
	}
	case 0x43: {// LD B,E
		cycles = 1;
		reg.B = reg.E;
		break;
	}
	case 0x44: {// LD B,H
		cycles = 1;
		reg.B = reg.H;
		break;
	}
	case 0x45: {// LD B,L
		cycles = 1;
		reg.B = reg.L;
		break;
	}
	case 0x46: {// LD B,(HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		reg.B = read(addr);
		break;
	}
	case 0x47: {// LD B, A
		cycles = 1;
		reg.B = reg.A;
		break;
	}
	case 0x48: {// LD C,B
		cycles = 1;
		reg.C = reg.B;
		break;
	}
	case 0x49: {// LD C,C
		cycles = 1;
		break;
	}
	case 0x4A: {// LD C,D
		cycles = 1;
		reg.C = reg.D;
		break;
	}
	case 0x4B: {// LD C,E
		cycles = 1;
		reg.C = reg.E;
		break;
	}
	case 0x4C: {// LD C,H
		cycles = 1;
		reg.C = reg.H;
		break;
	}
	case 0x4D: {// LD C,L
		cycles = 1;
		reg.C = reg.L;
		break;
	}
	case 0x4E: {// LD C,(HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		reg.C = read(addr);
		break;
	}
	case 0x4F: {// LD C,A
		cycles = 1;
		reg.C = reg.A;
		break;
	}
	case 0x50: {// LD D,B
		cycles = 1;
		reg.D = reg.B;
		break;
	}
	case 0x51: {// LD D,C
		cycles = 1;
		reg.D = reg.C;
		break;
	}
	case 0x52: {// LD D,D
		cycles = 1;
		break;
	}
	case 0x53: {// LD D,E
		cycles = 1;
		reg.D = reg.E;
		break;
	}
	case 0x54: {// LD D,H
		cycles = 1;
		reg.D = reg.H;
		break;
	}
	case 0x55: {// LD D,L
		cycles = 1;
		reg.D = reg.L;
		break;
	}
	case 0x56: {// LD D,(HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		reg.D = read(addr);
		break;
	}
	case 0x57: {// LD D,A
		cycles = 1;
		reg.D = reg.A;
		break;
	}
	case 0x58: {// LD E,B
		cycles = 1;
		reg.E = reg.B;
		break;
	}
	case 0x59: {// LD E,C
		cycles = 1;
		reg.E = reg.C;
		break;
	}
	case 0x5A: {// LD E,D
		cycles = 1;
		reg.E = reg.D;
		break;
	}
	case 0x5B: {// LD E,E
		cycles = 1;
		break;
	}
	case 0x5C: {// LD E,H
		cycles = 1;
		reg.E = reg.H;
		break;
	}
	case 0x5D: {// LD E,L
		cycles = 1;
		reg.E = reg.L;
		break;
	}
	case 0x5E: {//LD E,(HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		reg.E = read(addr);
		break;
	}
	case 0x5F: {// LD E,A
		cycles = 1;
		reg.E = reg.A;
		break;
	}
	case 0x60: {// LD H,B
		cycles = 1;
		reg.H = reg.B;
		break;
	}
	case 0x61: {// LD H,C
		cycles = 1;
		reg.H = reg.C;
		break;
	}
	case 0x62: {// LD H,D
		cycles = 1;
		reg.H = reg.D;
		break;
	}
	case 0x63: {// LD H,E
		cycles = 1;
		reg.H = reg.E;
		break;
	}
	case 0x64: {// LD H,H
		cycles = 1;
		break;
	}
	case 0x65: {// LD H,L
		cycles = 1;
		reg.H = reg.L;
		break;
	}
	case 0x66: {// LD H,(HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		reg.H = read(addr);
		break;
	}
	case 0x67: {// LD H,A
		cycles = 1;
		reg.H = reg.A;
		break;
	}
	case 0x68: {// LD L,B
		cycles = 1;
		reg.L = reg.B;
		break;
	}
	case 0x69: {// LD L,C
		cycles = 1;
		reg.L = reg.C;
		break;
	}
	case 0x6A: {// LD L,D
		cycles = 1;
		reg.L = reg.D;
		break;
	}
	case 0x6B: {// LD L,E
		cycles = 1;
		reg.L = reg.E;
		break;
	}
	case 0x6C: {// LD L,H
		cycles = 1;
		reg.L = reg.H;
		break;
	}
	case 0x6D: {// LD L,L
		cycles = 1;
		break;
	}
	case 0x6E: {//LD L,(HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		reg.L = read(addr);
		break;
	}
	case 0x6F: {// LD L,A
		cycles = 1;
		reg.L = reg.A;
		break;
	}
	case 0x70: {// LD (HL),B
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		write(addr, reg.B);
		break;
	}
	case 0x71: {// LD (HL),C
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		write(addr, reg.C);
		break;
	}
	case 0x72: {// LD (HL),D
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		write(addr, reg.D);
		break;
	}
	case 0x73: {// LD (HL),E
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		write(addr, reg.E);
		break;
	}
	case 0x74: {// LD (HL),H
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		write(addr, reg.H);
		break;
	}
	case 0x75: {// LD (HL),L
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		write(addr, reg.L);
		break;
	}
	case 0x76: {// HALT
		cycles = 1;
		is_halt = 1;
		break;
	}
	case 0x77: {// LD (HL),A
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		write(addr, reg.A);
		break;
	}
	case 0x78: {// LD A,B
		cycles = 1;
		reg.A = reg.B;
		break;
	}
	case 0x79: {// LD A,C
		cycles = 1;
		reg.A = reg.C;
		break;
	}
	case 0x7A: {// LD A,D
		cycles = 1;
		reg.A = reg.D;
		break;
	}
	case 0x7B: {// LD A,E
		cycles = 1;
		reg.A = reg.E;
		break;
	}
	case 0x7C: {// LD A,H
		cycles = 1;
		reg.A = reg.H;
		break;
	}
	case 0x7D: {// LD A,L
		cycles = 1;
		reg.A = reg.L;
		break;
	}
	case 0x7E: {//LD A,(HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		reg.A = read(addr);
		break;
	}
	case 0x7F: {//LD A,A
		cycles = 1;
		break;
	}
	case 0x80: {// ADD A,B
		cycles = 1;
		uint8_t res = reg.A + reg.B;
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.B & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.B) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x81: {// ADD A,C
		cycles = 1;
		uint8_t res = reg.A + reg.C;
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.C & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.C) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x82: {// ADD A,D
		cycles = 1;
		uint8_t res = reg.A + reg.D;
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.D & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.D) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x83: {// ADD A,E
		cycles = 1;
		uint8_t res = reg.A + reg.E;
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.E & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.E) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x84: {// ADD A,H
		cycles = 1;
		uint8_t res = reg.A + reg.H;
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.H & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.H) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x85: {// ADD A,L
		cycles = 1;
		uint8_t res = reg.A + reg.L;
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.L & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.L) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x86: {// ADD A,(HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		uint8_t res = reg.A + data;
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (data & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)data) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x87: {// ADD A,A
		cycles = 1;
		uint8_t res = reg.A + reg.A;
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.A & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.A) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x88: {// ADC A,B
		cycles = 1;
		uint8_t res = reg.A + reg.B + getFlag(C);
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.B & 0x0F) + getFlag(C)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.B + getFlag(C)) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x89: {// ADC A,C
		cycles = 1;
		uint8_t res = reg.A + reg.C + getFlag(C);
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.C & 0x0F) + getFlag(C)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.C + getFlag(C)) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x8A: {// ADC A,D
		cycles = 1;
		uint8_t res = reg.A + reg.D + getFlag(C);
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.D & 0x0F) + getFlag(C)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.D + getFlag(C)) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x8B: {// ADC A,E
		cycles = 1;
		uint8_t res = reg.A + reg.E + getFlag(C);
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.E & 0x0F) + getFlag(C)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.E + getFlag(C)) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x8C: {// ADC A,H
		cycles = 1;
		uint8_t res = reg.A + reg.H + getFlag(C);
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.H & 0x0F) + getFlag(C)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.H + getFlag(C)) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x8D: {// ADC A,L
		cycles = 1;
		uint8_t res = reg.A + reg.L + getFlag(C);
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.L & 0x0F) + getFlag(C)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.L + getFlag(C)) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x8E: {// ADD A,(HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		uint8_t res = reg.A + data + getFlag(C);
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (data & 0x0F) + getFlag(C)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)data + getFlag(C)) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x8F: {// ADC A,A
		cycles = 1;
		uint8_t res = reg.A + reg.A + getFlag(C);
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (reg.A & 0x0F) + getFlag(C)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)reg.A + getFlag(C)) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x90: {// SUB B
		cycles = 1;
		uint8_t res = reg.A - reg.B;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.B & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.B) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x91: {// SUB C
		cycles = 1;
		uint8_t res = reg.A - reg.C;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.C & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.C) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x92: {// SUB D
		cycles = 1;
		uint8_t res = reg.A - reg.D;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.D & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.D ) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x93: {// SUB E
		cycles = 1;
		uint8_t res = reg.A - reg.E;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.E & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.E ) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x94: {// SUB H
		cycles = 1;
		uint8_t res = reg.A - reg.H;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.H & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.H) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x95: {// SUB L
		cycles = 1;
		uint8_t res = reg.A - reg.L;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.L & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.L) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x96: {// SUB (HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		uint8_t res = reg.A - data;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (data & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)data) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x97: {// SUB A
		cycles = 1;
		uint8_t res = reg.A - reg.A;
		setFlag(N, 1);
		setFlag(H, 0);
		setFlag(C, 0);
		setFlag(Z, 1);
		reg.A = res;
		break;
	}
	case 0x98: {// SBC B
		cycles = 1;
		uint8_t CY = getFlag(C);
		uint8_t res = reg.A - reg.B - CY;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.B & 0x0F) - CY) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.B - CY) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x99: {// SBC C
		cycles = 1;
		uint8_t CY = getFlag(C);
		uint8_t res = reg.A - reg.C - CY;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.C & 0x0F) - CY) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.C - CY) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x9A: {// SBC D
		cycles = 1;
		uint8_t CY = getFlag(C);
		uint8_t res = reg.A - reg.D - CY;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.D & 0x0F) - CY) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.D - CY) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x9B: {// SBC E
		cycles = 1;
		uint8_t CY = getFlag(C);
		uint8_t res = reg.A - reg.E - CY;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.E & 0x0F) - CY) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.E - CY) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x9C: {// SBC H
		cycles = 1;
		uint8_t CY = getFlag(C);
		uint8_t res = reg.A - reg.H - CY;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.H & 0x0F) - CY) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.H - CY) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x9D: {// SBC L
		cycles = 1;
		uint8_t CY = getFlag(C);
		uint8_t res = reg.A - reg.L - CY;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.L & 0x0F) - CY) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.L - CY) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x9E: {// SBC (HL)
		cycles = 2;
		uint8_t CY = getFlag(C);
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		uint8_t res = reg.A - data - CY;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (data & 0x0F) - CY) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)data - CY) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0x9F: {// SBC A
		cycles = 1;
		uint8_t CY = getFlag(C);
		uint8_t res = reg.A - reg.A - CY;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (reg.A & 0x0F) - CY) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)reg.A - CY) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0xA0: {// AND B
		cycles = 1;
		reg.A &= reg.B;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 1);
		setFlag(C, 0);
		break;
	}
	case 0xA1: {// AND C
		cycles = 1;
		reg.A &= reg.C;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 1);
		setFlag(C, 0);
		break;
	}
	case 0xA2: {// AND D
		cycles = 1;
		reg.A &= reg.D;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 1);
		setFlag(C, 0);
		break;
	}
	case 0xA3: {// AND E
		cycles = 1;
		reg.A &= reg.E;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 1);
		setFlag(C, 0);
		break;
	}
	case 0xA4: {// AND H
		cycles = 1;
		reg.A &= reg.H;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 1);
		setFlag(C, 0);
		break;
	}
	case 0xA5: {// AND L
		cycles = 1;
		reg.A &= reg.L;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 1);
		setFlag(C, 0);
		break;
	}
	case 0xA6: {// AND (HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		reg.A &= read(addr);
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 1);
		setFlag(C, 0);
		break;
	}
	case 0xA7: {// AND A
		cycles = 1;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 1);
		setFlag(C, 0);
		break;
	}
	case 0xA8: {// XOR B
		cycles = 1;
		reg.A ^= reg.B;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xA9: {// XOR C
		cycles = 1;
		reg.A ^= reg.C;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xAA: {// XOR D
		cycles = 1;
		reg.A ^= reg.D;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xAB: {// XOR E
		cycles = 1;
		reg.A ^= reg.E;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xAC: {// XOR H
		cycles = 1;
		reg.A ^= reg.H;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xAD: {// XOR L
		cycles = 1;
		reg.A ^= reg.L;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xAE: {// XOR (HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		reg.A ^= read(addr);
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xAF: {// XOR A
		cycles = 1;
		reg.A = 0;
		setFlag(Z, 1);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xB0: {// OR B
		cycles = 1;
		reg.A |= reg.B;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xB1: {// OR C
		cycles = 1;
		reg.A |= reg.C;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xB2: {// OR D
		cycles = 1;
		reg.A |= reg.D;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xB3: {// OR E
		cycles = 1;
		reg.A |= reg.E;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xB4: {// OR H
		cycles = 1;
		reg.A |= reg.H;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xB5: {// OR L
		cycles = 1;
		reg.A |= reg.L;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xB6: {// OR (HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		reg.A |= read(addr);
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xB7: {// OR A
		cycles = 1;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xB8: {// CP B
		cycles = 1;
		setFlag(N, 1);
		setFlag(Z, reg.A == reg.B);
		setFlag(C, reg.A < reg.B);
		setFlag(H, (reg.A & 0x0F) < (reg.B & 0x0F));
		break;
	}
	case 0xB9: {// CP C
		cycles = 1;
		uint8_t tmp = reg.C;
		setFlag(N, 1);
		setFlag(Z, reg.A == tmp);
		setFlag(C, reg.A < tmp);
		setFlag(H, (reg.A & 0x0F) < (tmp & 0x0F));
		break;
	}
	case 0xBA: {// CP D
		cycles = 1;
		uint8_t tmp = reg.D;
		setFlag(N, 1);
		setFlag(Z, reg.A == tmp);
		setFlag(C, reg.A < tmp);
		setFlag(H, (reg.A & 0x0F) < (tmp & 0x0F));
		break;
	}
	case 0xBB: {// CP E
		cycles = 1;
		uint8_t tmp = reg.E;
		setFlag(N, 1);
		setFlag(Z, reg.A == tmp);
		setFlag(C, reg.A < tmp);
		setFlag(H, (reg.A & 0x0F) < (tmp & 0x0F));
		break;
	}
	case 0xBC: {// CP H
		cycles = 1;
		uint8_t tmp = reg.H;
		setFlag(N, 1);
		setFlag(Z, reg.A == tmp);
		setFlag(C, reg.A < tmp);
		setFlag(H, (reg.A & 0x0F) < (tmp & 0x0F));
		break;
	}
	case 0xBD: {// CP L
		cycles = 1;
		uint8_t tmp = reg.L;
		setFlag(N, 1);
		setFlag(Z, reg.A == tmp);
		setFlag(C, reg.A < tmp);
		setFlag(H, (reg.A & 0x0F) < (tmp & 0x0F));
		break;
	}
	case 0xBE: {// CP (HL)
		cycles = 2;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t tmp = read(addr);
		setFlag(N, 1);
		setFlag(Z, reg.A == tmp);
		setFlag(C, reg.A < tmp);
		setFlag(H, (reg.A & 0x0F) < (tmp & 0x0F));
		break;
	}
	case 0xBF: {// CP A
		cycles = 1;
		setFlag(N, 1);
		setFlag(Z, 1);
		setFlag(C, 0);
		setFlag(H, 0);
		break;
	}
	case 0xC0: {// RET NZ
		cycles = 2;
		if (!getFlag(Z)) {
			cycles += 3;
			uint8_t l = read(reg.SP++);
			uint16_t h = read(reg.SP++);
			reg.PC = ((h << 8) | l);
		}
		break;
	}
	case 0xC1: {// POP BC
		cycles = 3;
		reg.C = read(reg.SP++);
		reg.B = read(reg.SP++);
		break;
	}
	case 0xC2: {// JP NZ,a16
		cycles = 3;
		uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
		if (!getFlag(Z)) {
			cycles++;
			reg.PC = addr;
		}
		break;
	}
	case 0xC3: {// JP a16
		cycles = 4;
		uint8_t l = read(reg.PC++);
		uint16_t h = read(reg.PC++);
		uint16_t addr = (l | (h << 8));
		reg.PC = addr;
		break;
	}
	case 0xC4: {// CALL NZ, a16
		cycles = 3;
		uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
		if (!getFlag(Z)) {
			cycles += 3;
			write(--reg.SP, (reg.PC >> 8));
			write(--reg.SP, (reg.PC & 0x00FF));
			reg.PC = addr;
		}
		break;
	}
	case 0xC5: {// PUSH BC
		cycles = 4;
		write(--reg.SP, reg.B);
		write(--reg.SP, reg.C);
		break;
	}
	case 0xC6: {// ADD A,d8
		cycles = 2;
		uint8_t data = read(reg.PC++);
		uint8_t res = reg.A + data;
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (data & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A & 0xFF + (uint16_t)data & 0xFF) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0xC7: {// RST 00H
		cycles = 4;
		write(--reg.SP, (reg.PC >> 8));
		write(--reg.SP, (reg.PC & 0x00FF));
		reg.PC = 0x0000;
		break;
	}
	case 0xC8: {// RET Z
		cycles = 2;
		if (getFlag(Z)) {
			cycles += 3;
			uint8_t l = read(reg.SP++);
			uint16_t h = read(reg.SP++);
			reg.PC = ((h << 8) | l);
		}
		break;
	}
	case 0xC9: {// RET
		cycles = 4;
		uint8_t l = read(reg.SP++);
		uint16_t h = read(reg.SP++);
		reg.PC = ((h << 8) | l);
		break;
	}
	case 0xCA: {// JP Z,a16
		cycles = 3;
		uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
		if (getFlag(Z)) {
			cycles++;
			reg.PC = addr;
		}
		break;
	}
	case 0xCB: {//Prefix
		cycles = 1;
		uint8_t pre_op = read(reg.PC++);
		cycles += exec_prefixed(pre_op);
		break;
	}
	case 0xCC: {// CALL Z, a16
		cycles = 3;
		uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
		if (getFlag(Z)) {
			cycles += 3;
			write(--reg.SP, (reg.PC >> 8));
			write(--reg.SP, (reg.PC & 0x00FF));
			reg.PC = addr;
		}
		break;
	}
	case 0xCD: {// CALL a16
		cycles = 6;
		uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
		write(--reg.SP, (reg.PC >> 8));
		write(--reg.SP, (reg.PC & 0x00FF));
		reg.PC = addr;
		break;
	}
	case 0xCE: {// ADC A,d8
		cycles = 2;
		uint8_t data = read(reg.PC++);
		uint8_t res = reg.A + data + getFlag(C);
		setFlag(N, 0);
		setFlag(H, ((reg.A & 0x0F) + (data & 0x0F) + getFlag(C)) & 0x10);
		setFlag(C, ((uint16_t)reg.A + (uint16_t)data + getFlag(C)) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0xCF: {// RST 08H
		cycles = 4;
		write(--reg.SP, (reg.PC >> 8));
		write(--reg.SP, (reg.PC & 0x00FF));
		reg.PC = 0x0008;
		break;
	}
	case 0xD0: { // RET NC
		cycles = 2;
		if (!getFlag(C)) {
			cycles += 3;
			uint8_t l = read(reg.SP++);
			uint16_t h = read(reg.SP++);
			reg.PC = ((h << 8) | l);
		}
		break;
	}
	case 0xD1: {// POP DE
		cycles = 3;
		reg.E = read(reg.SP++);
		reg.D = read(reg.SP++);
		break;
	}
	case 0xD2: {// JP NC,a16
		cycles = 3;
		uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
		if (!getFlag(C)) {
			cycles++;
			reg.PC = addr;
		}
		break;
	}
	case 0xD3: { //UNDEFINED
		cycles = 1;
		break;
	}
	case 0xD4: {// CALL NC, a16
		cycles = 3;
		
		uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
		if (!getFlag(C)) {
			cycles += 3;
			write(--reg.SP, (reg.PC >> 8));
			write(--reg.SP, (reg.PC & 0x00FF));
			reg.PC = addr;
		}
		break;
	}
	case 0xD5: {// PUSH DE
		cycles = 4;
		write(--reg.SP, reg.D);
		write(--reg.SP, reg.E);
		break;
	}
	case 0xD6: {// SUB d8
		cycles = 2;
		uint8_t data = read(reg.PC++);
		uint8_t res = reg.A - data;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (data & 0x0F)) & 0x10);
		setFlag(C, ((uint16_t)reg.A & 0xFF - (uint16_t)data & 0xFF) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0xD7: {// RST 10H
		cycles = 4;
		write(--reg.SP, (reg.PC >> 8));
		write(--reg.SP, (reg.PC & 0x00FF));
		reg.PC = 0x0010;
		break;
	}
	case 0xD8: {// RET C
		cycles = 2;
		if (getFlag(C)) {
			cycles += 3;
			uint8_t l = read(reg.SP++);
			uint16_t h = read(reg.SP++);
			reg.PC = ((h << 8) | l);
		}
		break;
	}
	case 0xD9: {// RETI
		cycles = 4;
		uint8_t l = read(reg.SP++);
		uint16_t h = read(reg.SP++);
		reg.PC = ((h << 8) | l);
		IME = 1;
		break;
	}
	case 0xDA: {// JP C,a16
		cycles = 3;
		uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
		if (getFlag(C)) {
			cycles++;
			reg.PC = addr;
		}
		break;
	}
	case 0xDB: {//UNDEFINED
		cycles = 1;
		break;
	}
	case 0xDC: {// CALL C, a16
		cycles = 3;
		uint16_t addr = (read(reg.PC++) | ((uint16_t)read(reg.PC++) << 8));
		if (getFlag(C)) {
			cycles += 3;
			write(--reg.SP, (reg.PC >> 8));
			write(--reg.SP, (reg.PC & 0x00FF));
			reg.PC = addr;
		}
		break;
	}
	case 0xDD: {//UNDEFINED
		cycles = 1;
		break;
	}
	case 0xDE: {// SBC A, d8
		cycles = 2;
		uint8_t CY = getFlag(C);
		uint8_t data = read(reg.PC++);
		uint8_t res = reg.A - data - CY;
		setFlag(N, 1);
		setFlag(H, ((reg.A & 0x0F) - (data & 0x0F) - CY) & 0x10);
		setFlag(C, ((uint16_t)reg.A - (uint16_t)data - CY) & 0x100);
		setFlag(Z, res == 0);
		reg.A = res;
		break;
	}
	case 0xDF: {// RST 18H
		cycles = 4;
		write(--reg.SP, (reg.PC >> 8));
		write(--reg.SP, (reg.PC & 0x00FF));
		reg.PC = 0x0018;
		break;
	}
	case 0xE0: {// LDH (a8),A
		cycles = 3;
		uint8_t data = read(reg.PC++);
		uint16_t addr = 0xFF00 + data;
		write(addr, reg.A);
		break;
	}
	case 0xE1: {// POP HL
		cycles = 3;
		reg.L = read(reg.SP++);
		reg.H = read(reg.SP++);
		break;
	}
	case 0xE2: {//LD (C),A
		cycles = 2;
		uint16_t addr = 0xFF00 + reg.C;
		write(addr, reg.A);
		break;
	}
	case 0xE3: {//UNDEFINED
		cycles = 1;
		break;
	}
	case 0xE4: {//UNDEFINED
		cycles = 1;
		break;
	}
	case 0xE5: {// PUSH HL
		cycles = 4;
		write(--reg.SP, reg.H);
		write(--reg.SP, reg.L);
		break;
	}
	case 0xE6: {// AND d8
		cycles = 2;
		reg.A &= read(reg.PC++);
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 1);
		setFlag(C, 0);
		break;
	}
	case 0xE7: {// RST 20H
		cycles = 4;
		write(--reg.SP, (reg.PC >> 8));
		write(--reg.SP, (reg.PC & 0x00FF));
		reg.PC = 0x0020;
		break;
	}
	case 0xE8: {// ADD SP,r8
		cycles = 4;
		uint8_t data = read(reg.PC++);
		uint16_t res = reg.SP + data;
		setFlag(Z, 0);
		setFlag(N, 0);
		setFlag(H, ((reg.SP & 0x00FF) + data) & 0x100);
		setFlag(C, ((uint32_t)reg.SP + data) & 0x10000);
		reg.SP = res;
		break;
	}
	case 0xE9: {// JP HL
		cycles = 1;
		reg.PC = (((uint8_t)reg.H << 8) | reg.L);
		break;
	}
	case 0xEA: {// LD (a16),A
		cycles = 4;
		uint8_t l = read(reg.PC++);
		uint16_t h = read(reg.PC++);
		uint16_t addr = ((h << 8) | l);
		write(addr, reg.A);
		break;
	}
	case 0xEB: {//UNDEFINED
		cycles = 1;
		break;
	}
	case 0xEC: {//UNDEFINED
		cycles = 1;
		break;
	}
	case 0xED: {//UNDEFINED
		cycles = 1;
		break;
	}
	case 0xEE: {// XOR d8
		cycles = 2;
		reg.A ^= read(reg.PC++);
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xEF: {// RST 28H
		cycles = 4;
		write(--reg.SP, (reg.PC >> 8));
		write(--reg.SP, (reg.PC & 0x00FF));
		reg.PC = 0x0028;
		break;
	}
	case 0xF0: {// LDH A,(a8)
		cycles = 3;
		uint8_t data = read(reg.PC++);
		uint16_t addr = 0xFF00 + data;
		reg.A = read(addr);
		break;
	}
	case 0xF1: {// POP AF
		cycles = 3;
		reg.F = read(reg.SP++);
		reg.A = read(reg.SP++);
		break;
	}
	case 0xF2: {//LD A,(C)
		cycles = 2;
		uint16_t addr = 0xFF00 + reg.C;
		reg.A = read(addr);
		break;
	}
	case 0xF3: {// DI
		cycles = 1;
		IME = 0;
		break;
	}
	case 0xF4: {//UNDEFINED
		cycles = 1;
		break;
	}
	case 0xF5: {// PUSH AF
		cycles = 4;
		write(--reg.SP, reg.A);
		write(--reg.SP, reg.F);
		break;
	}
	case 0xF6: {// OR d8
		cycles = 2;
		reg.A |= read(reg.PC++);
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		setFlag(C, 0);
		break;
	}
	case 0xF7: {// RST 30H
		cycles = 4;
		write(--reg.SP, (reg.PC >> 8));
		write(--reg.SP, (reg.PC & 0x00FF));
		reg.PC = 0x0030;
		break;
	}
	case 0xF8: {// LD HL,SP+r8
		cycles = 3;
		uint8_t data = read(reg.PC++);
		uint16_t res = reg.SP + data;
		setFlag(Z, 0);
		setFlag(N, 0);
		setFlag(H, ((reg.SP & 0x00FF) + data) & 0x100);
		setFlag(C, ((uint32_t)reg.SP + data) & 0x10000);
		reg.H = (res >> 8);
		reg.L = (res & 0x00FF);
		break;
	}
	case 0xF9: {// LD SP,HL
		cycles = 2;
		reg.SP = (((uint16_t)reg.H << 8) | reg.L);
		break;
	}
	case 0xFA: {// LD A,(a16)
		cycles = 4;
		uint8_t l = read(reg.PC++);
		uint16_t h = read(reg.PC++);
		uint16_t addr = ((h << 8) | l);
		reg.A = read(addr);
		break;
	}
	case 0xFB: {// EI
		cycles = 1;
		IME = 1;
		break;
	}
	case 0xFC: {//UNDEFINED
		cycles = 1;
		break;
	}
	case 0xFD: {//UNDEFINED
		cycles = 1;
		break;
	}
	case 0xFE: {// CP d8
		cycles = 2;
		uint8_t tmp = read(reg.PC++);
		setFlag(N, 1);
		setFlag(Z, reg.A == tmp);
		setFlag(C, reg.A < tmp);
		setFlag(H, (reg.A & 0x0F) < (tmp & 0x0F));
		break;
	}
	case 0xFF: {// RST 38H
		cycles = 4;
		write(--reg.SP, (reg.PC >> 8));
		write(--reg.SP, (reg.PC & 0x00FF));
		reg.PC = 0x0038;
		break;
	}
	}
	return cycles;
}
