#include "CPU.h"

uint8_t CPU::exec_prefixed(uint8_t opcode) {
	uint8_t cycles = 0;
	switch (opcode)
	{
	case 0x0: {// RLC B
		cycles = 2;
		uint8_t CY = ((reg.B & 0x80) >> 7);
		reg.B = ((reg.B << 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.B == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x1: {// RLC C
		cycles = 2;
		uint8_t CY = ((reg.C & 0x80) >> 7);
		reg.C = ((reg.C << 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.C == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x2: {// RLC D
		cycles = 2;
		uint8_t CY = ((reg.D & 0x80) >> 7);
		reg.D = ((reg.D << 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.D == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x3: {// RLC E
		cycles = 2;
		uint8_t CY = ((reg.E & 0x80) >> 7);
		reg.E = ((reg.E << 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.E == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x4: {// RLC H
		cycles = 2;
		uint8_t CY = ((reg.H & 0x80) >> 7);
		reg.H = ((reg.H << 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.H == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x5: {// RLC L
		cycles = 2;
		uint8_t CY = ((reg.L & 0x80) >> 7);
		reg.L = ((reg.L << 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.L == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x6: {// RLC (HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		uint8_t CY = ((data & 0x80) >> 7);
		data = ((data << 1) | CY);
		setFlag(C, CY);
		setFlag(Z, data == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		write(addr, data);
		break;
	}
	case 0x7: {// RLC A
		cycles = 2;
		uint8_t CY = ((reg.A & 0x80) >> 7);
		reg.A = ((reg.A << 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x8: {// RRC B
		cycles = 2;
		uint8_t CY = ((reg.B & 0x01) << 7);
		reg.B = ((reg.B >> 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.B == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x9: {// RRC C
		cycles = 2;
		uint8_t CY = ((reg.C & 0x01) << 7);
		reg.C = ((reg.C >> 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.C == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0xA: {// RRC D
		cycles = 2;
		uint8_t CY = ((reg.D & 0x01) << 7);
		reg.D = ((reg.D >> 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.D == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0xB: {// RRC E
		cycles = 2;
		uint8_t CY = ((reg.E & 0x01) << 7);
		reg.E = ((reg.E >> 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.E == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0xC: {// RRC H
		cycles = 2;
		uint8_t CY = ((reg.H & 0x01) << 7);
		reg.H = ((reg.H >> 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.H == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0xD: {// RRC L
		cycles = 2;
		uint8_t CY = ((reg.L & 0x01) << 7);
		reg.L = ((reg.L >> 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.L == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0xE: {// RRC (HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		uint8_t CY = ((data & 0x01) << 7);
		data = ((data >> 1) | CY);
		setFlag(C, CY);
		setFlag(Z, data == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		write(addr, data);
		break;
	}
	case 0xF: {// RRC A
		cycles = 2;
		uint8_t CY = ((reg.A & 0x01) << 7);
		reg.A = ((reg.A >> 1) | CY);
		setFlag(C, CY);
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x10: {// RL B
		cycles = 2;
		uint8_t CY = getFlag(C);
		setFlag(C, (reg.B & 0x80) > 0);
		reg.B = ((reg.B << 1) | CY);
		setFlag(Z, reg.B == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x11: {// RL C
		cycles = 2;
		uint8_t CY = getFlag(C);
		setFlag(C, (reg.C & 0x80) > 0);
		reg.C = ((reg.C << 1) | CY);
		setFlag(Z, reg.C == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x12: {// RL D
		cycles = 2;
		uint8_t CY = getFlag(C);
		setFlag(C, (reg.D & 0x80) > 0);
		reg.D = ((reg.D << 1) | CY);
		setFlag(Z, reg.D == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x13: {// RL E
		cycles = 2;
		uint8_t CY = getFlag(C);
		setFlag(C, (reg.E & 0x80) > 0);
		reg.E = ((reg.B << 1) | CY);
		setFlag(Z, reg.E == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x14: {// RL H
		cycles = 2;
		uint8_t CY = getFlag(C);
		setFlag(C, (reg.H & 0x80) > 0);
		reg.H = ((reg.H << 1) | CY);
		setFlag(Z, reg.H == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x15: {// RL L
		cycles = 2;
		uint8_t CY = getFlag(C);
		setFlag(C, (reg.L & 0x80) > 0);
		reg.L = ((reg.L << 1) | CY);
		setFlag(Z, reg.L == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x16: {// RL (HL)
		cycles = 4;
		uint8_t CY = getFlag(C);
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		setFlag(C, (addr & 0x80) > 0);
		addr = ((addr << 1) | CY);
		setFlag(Z, addr == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		write(addr, data);
		break;
	}
	case 0x17: {// RL A
		cycles = 2;
		uint8_t CY = getFlag(C);
		setFlag(C, (reg.A & 0x80) > 0);
		reg.A = ((reg.A << 1) | CY);
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x18: {// RR B
		cycles = 2;
		uint8_t CY = (getFlag(C)<<7);
		setFlag(C, (reg.B & 0x01) > 0);
		reg.B = ((reg.B >> 1) | CY);
		setFlag(Z, reg.B == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x19: {// RR C
		cycles = 2;
		uint8_t CY = (getFlag(C) << 7);
		setFlag(C, (reg.C & 0x01) > 0);
		reg.C = ((reg.C >> 1) | CY);
		setFlag(Z, reg.C == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x1A: {// RR D
		cycles = 2;
		uint8_t CY = (getFlag(C) << 7);
		setFlag(C, (reg.D & 0x01) > 0);
		reg.D = ((reg.D >> 1) | CY);
		setFlag(Z, reg.D == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x1B: {// RR E
		cycles = 2;
		uint8_t CY = (getFlag(C) << 7);
		setFlag(C, (reg.E & 0x01) > 0);
		reg.E = ((reg.E >> 1) | CY);
		setFlag(Z, reg.E == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x1C: {// RR H
		cycles = 2;
		uint8_t CY = (getFlag(C) << 7);
		setFlag(C, (reg.H & 0x01) > 0);
		reg.H = ((reg.H >> 1) | CY);
		setFlag(Z, reg.H == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x1D: {// RR L
		cycles = 2;
		uint8_t CY = (getFlag(C) << 7);
		setFlag(C, (reg.L & 0x01) > 0);
		reg.L = ((reg.L >> 1) | CY);
		setFlag(Z, reg.L == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x1E: {// RR (HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		uint8_t CY = (getFlag(C) << 7);
		setFlag(C, (data & 0x01) > 0);
		data = ((data >> 1) | CY);
		setFlag(Z, data == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		write(addr, data);
		break;
	}
	case 0x1F: {// RR A
		cycles = 2;
		uint8_t CY = (getFlag(C) << 7);
		setFlag(C, (reg.A & 0x01) > 0);
		reg.A = ((reg.A >> 1) | CY);
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x20: {// SLA B
		cycles = 2;
		setFlag(C, ((reg.B & 0x80) >> 7));
		reg.B <<= 1;
		setFlag(Z, reg.B == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x21: {// SLA C
		cycles = 2;
		setFlag(C, ((reg.C & 0x80) >> 7));
		reg.C <<= 1;
		setFlag(Z, reg.C == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x22: {// SLA D
		cycles = 2;
		setFlag(C, ((reg.D & 0x80) >> 7));
		reg.D <<= 1;
		setFlag(Z, reg.D == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x23: {// SLA E
		cycles = 2;
		setFlag(C, ((reg.E & 0x80) >> 7));
		reg.E <<= 1;
		setFlag(Z, reg.E == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x24: {// SLA H
		cycles = 2;
		setFlag(C, ((reg.H & 0x80) >> 7));
		reg.H <<= 1;
		setFlag(Z, reg.H == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x25: {// SLA L
		cycles = 2;
		setFlag(C, ((reg.L & 0x80) >> 7));
		reg.L <<= 1;
		setFlag(Z, reg.L == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x26: {// SLA (HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		setFlag(C, ((data & 0x80) >> 7));
		data <<= 1;
		setFlag(Z, data == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		write(addr, data);
		break;
	}
	case 0x27: {// SLA A
		cycles = 2;
		setFlag(C, ((reg.A & 0x80) >> 7));
		reg.A <<= 1;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x28: {// SRA B
		cycles = 2;
		setFlag(C, (reg.B & 0x01));
		reg.B = ((reg.B >> 1) | (reg.B & 0x80));
		setFlag(Z, reg.B == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x29: {// SRA C
		cycles = 2;
		setFlag(C, (reg.C & 0x01));
		reg.C = ((reg.C >> 1) | (reg.C & 0x80));
		setFlag(Z, reg.C == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x2A: {// SRA D
		cycles = 2;
		setFlag(C, (reg.D & 0x01));
		reg.D = ((reg.D >> 7) | (reg.D & 0x80));
		setFlag(Z, reg.D == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x2B: {// SRA E
		cycles = 2;
		setFlag(C, (reg.E & 0x01));
		reg.E = ((reg.E >> 7) | (reg.E & 0x80)); // LOOOOOOOOOOOOOOL wtf why >>7 !!!????
		setFlag(Z, reg.E == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x2C: {// SRA H
		cycles = 2;
		setFlag(C, (reg.H & 0x01));
		reg.H = ((reg.H >> 7) | (reg.H & 0x80));
		setFlag(Z, reg.H == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x2D: {// SRA L
		cycles = 2;
		setFlag(C, (reg.L & 0x01));
		reg.L = ((reg.L >> 7) | (reg.L & 0x80));
		setFlag(Z, reg.L == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x2E: {// SRA (HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		setFlag(C, (data & 0x01));
		data = ((data >> 7) | (data & 0x80));
		setFlag(Z, data == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		write(addr,data);
		break;
	}
	case 0x2F: {// SRA A
		cycles = 2;
		setFlag(C, (reg.A & 0x01));
		reg.A = ((reg.A >> 7) | (reg.A & 0x80));
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x30: {// SWAP B
		cycles = 2;
		reg.B = ((reg.B << 4) | (reg.B >> 4));
		setFlag(Z, reg.B == 0);
		setFlag(H, 0);
		setFlag(N, 0);
		setFlag(C, 0);
		break;
	}
	case 0x31: {// SWAP C
		cycles = 2;
		reg.C = ((reg.C << 4) | (reg.C >> 4));
		setFlag(Z, reg.C == 0);
		setFlag(H, 0);
		setFlag(N, 0);
		setFlag(C, 0);
		break;
	}
	case 0x32: {// SWAP D
		cycles = 2;
		reg.D = ((reg.D << 4) | (reg.D >> 4));
		setFlag(Z, reg.D == 0);
		setFlag(H, 0);
		setFlag(N, 0);
		setFlag(C, 0);
		break;
	}
	case 0x33: {// SWAP E
		cycles = 2;
		reg.E = ((reg.E << 4) | (reg.E >> 4));
		setFlag(Z, reg.E == 0);
		setFlag(H, 0);
		setFlag(N, 0);
		setFlag(C, 0);
		break;
	}
	case 0x34: {// SWAP H
		cycles = 2;
		reg.H = ((reg.H << 4) | (reg.H >> 4));
		setFlag(Z, reg.H == 0);
		setFlag(H, 0);
		setFlag(N, 0);
		setFlag(C, 0);
		break;
	}
	case 0x35: {// SWAP L
		cycles = 2;
		reg.L = ((reg.L << 4) | (reg.L >> 4));
		setFlag(Z, reg.L == 0);
		setFlag(H, 0);
		setFlag(N, 0);
		setFlag(C, 0);
		break;
	}
	case 0x36: {// SWAP (HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data = ((data << 4) | (data >> 4));
		setFlag(Z, reg.B == 0);
		setFlag(H, 0);
		setFlag(N, 0);
		setFlag(C, 0);
		write(addr, data);
		break;
	}
	case 0x37: {// SWAP A
		cycles = 2;
		reg.A = ((reg.A << 4) | (reg.A >> 4));
		setFlag(Z, reg.A == 0);
		setFlag(H, 0);
		setFlag(N, 0);
		setFlag(C, 0);
		break;
	}
	case 0x38: {//SRL B
		cycles = 2;
		setFlag(C, (reg.B & 0x01));
		reg.B >>= 1;
		setFlag(Z, reg.B == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x39: {//SRL C
		cycles = 2;
		setFlag(C, (reg.C & 0x01));
		reg.C >>= 1;
		setFlag(Z, reg.C == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x3A: {//SRL D
		cycles = 2;
		setFlag(C, (reg.D & 0x01));
		reg.D >>= 1;
		setFlag(Z, reg.D == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x3B: {//SRL E
		cycles = 2;
		setFlag(C, (reg.E & 0x01));
		reg.E >>= 1;
		setFlag(Z, reg.E == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x3C: {//SRL H
		cycles = 2;
		setFlag(C, (reg.H & 0x01));
		reg.H >>= 1;
		setFlag(Z, reg.H == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x3D: {//SRL L
		cycles = 2;
		setFlag(C, (reg.L & 0x01));
		reg.L >>= 1;
		setFlag(Z, reg.L == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x3E: {//SRL (HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		setFlag(C, (data & 0x01));
		data >>= 1;
		setFlag(Z, data == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		write(addr, data);
		break;
	}
	case 0x3F: {//SRL A
		cycles = 2;
		setFlag(C, (reg.A & 0x01));
		reg.A >>= 1;
		setFlag(Z, reg.A == 0);
		setFlag(N, 0);
		setFlag(H, 0);
		break;
	}
	case 0x40: {// BIT 0,B
		cycles = 2;
		bool b = ((reg.B & (1 << 0)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x41: {// BIT 0,C
		cycles = 2;
		bool b = ((reg.C & (1 << 0)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x42: {// BIT 0,D
		cycles = 2;
		bool b = ((reg.D & (1 << 0)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x43: {// BIT 0,E
		cycles = 2;
		bool b = ((reg.E & (1 << 0)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x44: {// BIT 0,H
		cycles = 2;
		bool b = ((reg.H & (1 << 0)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x45: {// BIT 0,L
		cycles = 2;
		bool b = ((reg.L & (1 << 0)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x46: {// BIT 0, (HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		bool b = ((read(addr) & (1 << 0)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x47: {// BIT 0,A
		cycles = 2;
		bool b = ((reg.A & (1 << 0)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x48: {// BIT 1,B
		cycles = 2;
		bool b = ((reg.B & (1 << 1)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x49: {// BIT 1,C
		cycles = 2;
		bool b = ((reg.C & (1 << 1)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x4A: {// BIT 1,D
		cycles = 2;
		bool b = ((reg.D & (1 << 1)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x4B: {// BIT 1,E
		cycles = 2;
		bool b = ((reg.E & (1 << 1)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x4C: {// BIT 1,H
		cycles = 2;
		bool b = ((reg.H & (1 << 1)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x4D: {// BIT 1,L
		cycles = 2;
		bool b = ((reg.L & (1 << 1)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x4E: {// BIT 1,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		bool b = ((read(addr) & (1 << 1)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x4F: {// BIT 1,A
		cycles = 2;
		bool b = ((reg.A & (1 << 1)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x50: {// BIT 2,B
		cycles = 2;
		bool b = ((reg.B & (1 << 2)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x51: {// BIT 2,C
		cycles = 2;
		bool b = ((reg.C & (1 << 2)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x52: {// BIT 2,D
		cycles = 2;
		bool b = ((reg.D & (1 << 2)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x53: {// BIT 2,E
		cycles = 2;
		bool b = ((reg.E & (1 << 2)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x54: {// BIT 2,H
		cycles = 2;
		bool b = ((reg.H & (1 << 2)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x55: {// BIT 2,L
		cycles = 2;
		bool b = ((reg.L & (1 << 2)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x56: {// BIT 2,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		bool b = ((read(addr) & (1 << 2)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x57: {// BIT 2,A
		cycles = 2;
		bool b = ((reg.A & (1 << 2)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x58: {// BIT 3,B
		cycles = 2;
		bool b = ((reg.B & (1 << 3)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x59: {// BIT 3,C
		cycles = 2;
		bool b = ((reg.C & (1 << 3)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x5A: {// BIT 3,D
		cycles = 2;
		bool b = ((reg.D & (1 << 3)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x5B: {// BIT 3,E
		cycles = 2;
		bool b = ((reg.E & (1 << 3)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x5C: {// BIT 3,H
		cycles = 2;
		bool b = ((reg.H & (1 << 3)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x5D: {// BIT 3,L
		cycles = 2;
		bool b = ((reg.L & (1 << 3)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x5E: {// BIT 3,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		bool b = ((read(addr) & (1 << 3)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x5F: {// BIT 3,A
		cycles = 2;
		bool b = ((reg.A & (1 << 3)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x60: {// BIT 4,B
		cycles = 2;
		bool b = ((reg.B & (1 << 4)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x61: {// BIT 4,C
		cycles = 2;
		bool b = ((reg.C & (1 << 4)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x62: {// BIT 4,D
		cycles = 2;
		bool b = ((reg.D & (1 << 4)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
	}
	case 0x63: {// BIT 4,E
		cycles = 2;
		bool b = ((reg.E & (1 << 4)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x64: {// BIT 4,H
		cycles = 2;
		bool b = ((reg.H & (1 << 4)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x65: {// BIT 4,H
		cycles = 2;
		bool b = ((reg.H & (1 << 4)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x66: {// BIT 4,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		bool b = ((read(addr) & (1 << 4)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x67: {// BIT 4,A
		cycles = 2;
		bool b = ((reg.A & (1 << 4)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x68: {// BIT 5,B
		cycles = 2;
		bool b = ((reg.B & (1 << 5)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x69: {// BIT 5,C
		cycles = 2;
		bool b = ((reg.C & (1 << 5)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x6A: {// BIT 5,D
		cycles = 2;
		bool b = ((reg.D & (1 << 5)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x6B: {// BIT 5,E
		cycles = 2;
		bool b = ((reg.E & (1 << 5)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x6C: {// BIT 5,H
		cycles = 2;
		bool b = ((reg.H & (1 << 5)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x6D: {// BIT 5,L
		cycles = 2;
		bool b = ((reg.L & (1 << 5)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x6E: {// BIT 5,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		bool b = ((read(addr) & (1 << 5)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x6F: {// BIT 5,A
		cycles = 2;
		bool b = ((reg.A & (1 << 5)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x70: {// BIT 6,B
		cycles = 2;
		bool b = ((reg.B & (1 << 6)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x71: {// BIT 6,C
		cycles = 2;
		bool b = ((reg.C & (1 << 6)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x72: {// BIT 6,D
		cycles = 2;
		bool b = ((reg.D & (1 << 6)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
	}
	case 0x73: {// BIT 6,E
		cycles = 2;
		bool b = ((reg.E & (1 << 6)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x74: {// BIT 6,H
		cycles = 2;
		bool b = ((reg.H & (1 << 6)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x75: {// BIT 6,L
		cycles = 2;
		bool b = ((reg.L & (1 << 6)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x76: {// BIT 6,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		bool b = ((read(addr) & (1 << 6)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x77: {// BIT 6,A
		cycles = 2;
		bool b = ((reg.A & (1 << 6)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x78: {// BIT 7,B
		cycles = 2;
		bool b = ((reg.B & (1 << 7)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x79: {// BIT 7,C
		cycles = 2;
		bool b = ((reg.C & (1 << 7)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x7A: {// BIT 7,D
		cycles = 2;
		bool b = ((reg.D & (1 << 7)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x7B: {// BIT 7,E
		cycles = 2;
		bool b = ((reg.E & (1 << 7)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x7C: {// BIT 7,H
		cycles = 2;
		bool b = ((reg.H & (1 << 7)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x7D: {// BIT 7,L
		cycles = 2;
		bool b = ((reg.L & (1 << 7)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x7E: {// BIT 7,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		bool b = ((read(addr) & (1 << 7)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x7F: {// BIT 7,A
		cycles = 2;
		bool b = ((reg.A & (1 << 7)) == 0);
		setFlag(Z, b);
		setFlag(N, 0);
		setFlag(H, 1);
		break;
	}
	case 0x80: {// RES 0,B
		cycles = 2;
		reg.B &= ~(1 << 0);
		break;
	}
	case 0x81: {// RES 0,C
		cycles = 2;
		reg.C &= ~(1 << 0);
		break;
	}
	case 0x82: {// RES 0,D
		cycles = 2;
		reg.D &= ~(1 << 0);
		break;
	}
	case 0x83: {// RES 0,E
		cycles = 2;
		reg.E &= ~(1 << 0);
		break;
	}
	case 0x84: {// RES 0,H
		cycles = 2;
		reg.H &= ~(1 << 0);
		break;
	}
	case 0x85: {// RES 0,L
		cycles = 2;
		reg.L &= ~(1 << 0);
		break;
	}
	case 0x86: {// RES 0,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data &= ~(1 << 0);
		write(addr, data);
		break;
	}
	case 0x87: {// RES 0,A
		cycles = 2;
		reg.A &= ~(1 << 0);
		break;
	}
	case 0x88: {// RES 1,B
		cycles = 2;
		reg.B &= ~(1 << 1);
		break;
	}
	case 0x89: {// RES 1,C
		cycles = 2;
		reg.C &= ~(1 << 1);
		break;
	}
	case 0x8A: {// RES 1,D
		cycles = 2;
		reg.D &= ~(1 << 1);
		break;
	}
	case 0x8B: {// RES 1,E
		cycles = 2;
		reg.E &= ~(1 << 1);
		break;
	}
	case 0x8C: {// RES 1,H
		cycles = 2;
		reg.H &= ~(1 << 1);
		break;
	}
	case 0x8D: {// RES 1,L
		cycles = 2;
		reg.L &= ~(1 << 1);
		break;
	}
	case 0x8E: {// RES 1,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data &= ~(1 << 1);
		write(addr, data);
		break;
	}
	case 0x8F: {// RES 1,A
		cycles = 2;
		reg.A &= ~(1 << 1);
		break;
	}
	case 0x90: {// RES 2,B
		cycles = 2;
		reg.B &= ~(1 << 2);
		break;
	}
	case 0x91: {// RES 2,C
		cycles = 2;
		reg.C &= ~(1 << 2);
		break;
	}
	case 0x92: {// RES 2,D
		cycles = 2;
		reg.D &= ~(1 << 2);
		break;
	}
	case 0x93: {// RES 2,E
		cycles = 2;
		reg.E &= ~(1 << 2);
		break;
	}
	case 0x94: {// RES 2,H
		cycles = 2;
		reg.H &= ~(1 << 2);
		break;
	}
	case 0x95: {// RES 2,L
		cycles = 2;
		reg.L &= ~(1 << 2);
		break;
	}
	case 0x96: {// RES 2,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data &= ~(1 << 2);
		write(addr, data);
		break;
	}
	case 0x97: {// RES 2,A
		cycles = 2;
		reg.A &= ~(1 << 2);
		break;
	}
	case 0x98: {// RES 3,B
		cycles = 2;
		reg.B &= ~(1 << 3);
		break;
	}
	case 0x99: {// RES 3,C
		cycles = 2;
		reg.C &= ~(1 << 3);
		break;
	}
	case 0x9A: {// RES 3,D
		cycles = 2;
		reg.D &= ~(1 << 3);
		break;
	}
	case 0x9B: {// RES 3,E
		cycles = 2;
		reg.E &= ~(1 << 3);
		break;
	}
	case 0x9C: {// RES 3,H
		cycles = 2;
		reg.H &= ~(1 << 3);
		break;
	}
	case 0x9D: {// RES 3,L
		cycles = 2;
		reg.L &= ~(1 << 3);
		break;
	}
	case 0x9E: {// RES 3,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data &= ~(1 << 3);
		write(addr, data);
		break;
	}
	case 0x9F: {// RES 3,A
		cycles = 2;
		reg.A &= ~(1 << 3);
		break;
	}
	case 0xA0: {// RES 4,B
		cycles = 2;
		reg.B &= ~(1 << 4);
		break;
	}
	case 0xA1: {// RES 4,C
		cycles = 2;
		reg.C &= ~(1 << 4);
		break;
	}
	case 0xA2: {// RES 4,D
		cycles = 2;
		reg.D &= ~(1 << 4);
		break;
	}
	case 0xA3: {// RES 4,E
		cycles = 2;
		reg.E &= ~(1 << 4);
		break;
	}
	case 0xA4: {// RES 4,H
		cycles = 2;
		reg.H &= ~(1 << 4);
		break;
	}
	case 0xA5: {// RES 4,L
		cycles = 2;
		reg.L &= ~(1 << 4);
		break;
	}
	case 0xA6: {// RES 4,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data &= ~(1 << 4);
		write(addr, data);
		break;
	}
	case 0xA7: {// RES 4,A
		cycles = 2;
		reg.A &= ~(1 << 4);
		break;
	}
	case 0xA8: {// RES 5,B
		cycles = 2;
		reg.B &= ~(1 << 5);
		break;
	}
	case 0xA9: {// RES 5,C
		cycles = 2;
		reg.C &= ~(1 << 5);
		break;
	}
	case 0xAA: {// RES 5,D
		cycles = 2;
		reg.D &= ~(1 << 5);
		break;
	}
	case 0xAB: {// RES 5,E
		cycles = 2;
		reg.E &= ~(1 << 5);
		break;
	}
	case 0xAC: {// RES 5,H
		cycles = 2;
		reg.H &= ~(1 << 5);
		break;
	}
	case 0xAD: {// RES 5,L
		cycles = 2;
		reg.L &= ~(1 << 5);
		break;
	}
	case 0xAE: {// RES 5,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data &= ~(1 << 5);
		write(addr, data);
		break;
	}
	case 0xAF: {// RES 5,A
		cycles = 2;
		reg.A &= ~(1 << 5);
		break;
	}
	case 0xB0: {// RES 6,B
		cycles = 2;
		reg.B &= ~(1 << 6);
		break;
	}
	case 0xB1: {// RES 6,C
		cycles = 2;
		reg.C &= ~(1 << 6);
		break;
	}
	case 0xB2: {// RES 6,D
		cycles = 2;
		reg.D &= ~(1 << 6);
		break;
	}
	case 0xB3: {// RES 6,E
		cycles = 2;
		reg.E &= ~(1 << 6);
		break;
	}
	case 0xB4: {// RES 6,H
		cycles = 2;
		reg.H &= ~(1 << 6);
		break;
	}
	case 0xB5: {// RES 6,L
		cycles = 2;
		reg.L &= ~(1 << 6);
		break;
	}
	case 0xB6: {// RES 6,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data &= ~(1 << 6);
		write(addr, data);
		break;
	}
	case 0xB7: {// RES 6,A
		cycles = 2;
		reg.A &= ~(1 << 6);
		break;
	}
	case 0xB8: {// RES 7,B
		cycles = 2;
		reg.B &= ~(1 << 7);
		break;
	}
	case 0xB9: {// RES 7,C
		cycles = 2;
		reg.C &= ~(1 << 7);
		break;
	}
	case 0xBA: {// RES 7,D
		cycles = 2;
		reg.D &= ~(1 << 7);
		break;
	}
	case 0xBB: {// RES 7,E
		cycles = 2;
		reg.E &= ~(1 << 7);
		break;
	}
	case 0xBC: {// RES 7,H
		cycles = 2;
		reg.H &= ~(1 << 7);
		break;
	}
	case 0xBD: {// RES 7,L
		cycles = 2;
		reg.L &= ~(1 << 7);
		break;
	}
	case 0xBE: {// RES 7,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data &= ~(1 << 7);
		write(addr, data);
		break;
	}
	case 0xBF: {// RES 7,A
		cycles = 2;
		reg.A &= ~(1 << 7);
		break;
	}
	case 0xC0: {// SET 0,B
		cycles = 2;
		reg.B |= (1 << 0);
		break;
	}
	case 0xC1: {// SET 0,C
		cycles = 2;
		reg.C |= (1 << 0);
		break;
	}
	case 0xC2: {// SET 0,D
		cycles = 2;
		reg.D |= (1 << 0);
		break;
	}
	case 0xC3: {// SET 0,E
		cycles = 2;
		reg.E |= (1 << 0);
		break;
	}
	case 0xC4: {// SET 0,H
		cycles = 2;
		reg.H |= (1 << 0);
		break;
	}
	case 0xC5: {// SET 0,L
		cycles = 2;
		reg.L |= (1 << 0);
		break;
	}
	case 0xC6: {// SET 0,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data |= (1 << 0);
		write(addr, data);
		break;
	}
	case 0xC7: {// SET 0,A
		cycles = 2;
		reg.A |= (1 << 0);
		break;
	}
	case 0xC8: {// SET 1,B
		cycles = 2;
		reg.B |= (1 << 1);
		break;
	}
	case 0xC9: {// SET 1,C
		cycles = 2;
		reg.C |= (1 << 1);
		break;
	}
	case 0xCA: {// SET 1,D
		cycles = 2;
		reg.D |= (1 << 1);
		break;
	}
	case 0xCB: {// SET 1,E
		cycles = 2;
		reg.E |= (1 << 1);
		break;
	}
	case 0xCC: {// SET 1,H
		cycles = 2;
		reg.H |= (1 << 1);
		break;
	}
	case 0xCD: {// SET 1,L
		cycles = 2;
		reg.L |= (1 << 1);
		break;
	}
	case 0xCE: {// SET 1,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data |= (1 << 1);
		write(addr, data);
		break;
	}
	case 0xCF: {// SET 1,A
		cycles = 2;
		reg.A |= (1 << 1);
		break;
	}
	case 0xD0: {// SET 2,B
		cycles = 2;
		reg.B |= (1 << 2);
		break;
	}
	case 0xD1: {// SET 2,C
		cycles = 2;
		reg.C |= (1 << 2);
		break;
	}
	case 0xD2: {// SET 2,D
		cycles = 2;
		reg.D |= (1 << 2);
		break;
	}
	case 0xD3: {// SET 2,E
		cycles = 2;
		reg.E |= (1 << 2);
		break;
	}
	case 0xD4: {// SET 2,H
		cycles = 2;
		reg.H |= (1 << 2);
		break;
	}
	case 0xD5: {// SET 2,L
		cycles = 2;
		reg.L |= (1 << 2);
		break;
	}
	case 0xD6: {// SET 2,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data |= (1 << 2);
		write(addr, data);
		break;
	}
	case 0xD7: {// SET 2,A
		cycles = 2;
		reg.A |= (1 << 2);
		break;
	}
	case 0xD8: {// SET 3,B
		cycles = 2;
		reg.B |= (1 << 3);
		break;
	}
	case 0xD9: {// SET 3,C
		cycles = 2;
		reg.C |= (1 << 3);
		break;
	}
	case 0xDA: {// SET 3,D
		cycles = 2;
		reg.D |= (1 << 3);
		break;
	}
	case 0xDB: {// SET 3,E
		cycles = 2;
		reg.E |= (1 << 3);
		break;
	}
	case 0xDC: {// SET 3,H
		cycles = 2;
		reg.H |= (1 << 3);
		break;
	}
	case 0xDD: {// SET 3,L
		cycles = 2;
		reg.L |= (1 << 3);
		break;
	}
	case 0xDE: {// SET 3,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data |= (1 << 3);
		write(addr, data);
		break;
	}
	case 0xDF: {// SET 3,A
		cycles = 2;
		reg.A |= (1 << 3);
		break;
	}
	case 0xE0: {// SET 4,B
		cycles = 2;
		reg.B |= (1 << 4);
		break;
	}
	case 0xE1: {// SET 4,C
		cycles = 2;
		reg.C |= (1 << 4);
		break;
	}
	case 0xE2: {// SET 4,D
		cycles = 2;
		reg.D |= (1 << 4);
		break;
	}
	case 0xE3: {// SET 4,E
		cycles = 2;
		reg.E |= (1 << 4);
		break;
	}
	case 0xE4: {// SET 4,H
		cycles = 2;
		reg.H |= (1 << 4);
		break;
	}
	case 0xE5: {// SET 4,L
		cycles = 2;
		reg.L |= (1 << 4);
		break;
	}
	case 0xE6: {// SET 4,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data |= (1 << 4);
		write(addr, data);
		break;
	}
	case 0xE7: {// SET 4,A
		cycles = 2;
		reg.A |= (1 << 4);
		break;
	}
	case 0xE8: {// SET 5,B
		cycles = 2;
		reg.B |= (1 << 5);
		break;
	}
	case 0xE9: {// SET 5,C
		cycles = 2;
		reg.C |= (1 << 5);
		break;
	}
	case 0xEA: {// SET 5,D
		cycles = 2;
		reg.D |= (1 << 5);
		break;
	}
	case 0xEB: {// SET 5,E
		cycles = 2;
		reg.E |= (1 << 5);
		break;
	}
	case 0xEC: {// SET 5,H
		cycles = 2;
		reg.H |= (1 << 5);
		break;
	}
	case 0xED: {// SET 5,L
		cycles = 2;
		reg.L |= (1 << 5);
		break;
	}
	case 0xEE: {// SET 5,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data |= (1 << 5);
		write(addr, data);
		break;
	}
	case 0xEF: {// SET 5,A
		cycles = 2;
		reg.A |= (1 << 5);
		break;
	}
	case 0xF0: {// SET 6,B
		cycles = 2;
		reg.B |= (1 << 6);
		break;
	}
	case 0xF1: {// SET 6,C
		cycles = 2;
		reg.C |= (1 << 6);
		break;
	}
	case 0xF2: {// SET 6,D
		cycles = 2;
		reg.D |= (1 << 6);
		break;
	}
	case 0xF3: {// SET 6,E
		cycles = 2;
		reg.E |= (1 << 6);
		break;
	}
	case 0xF4: {// SET 6,H
		cycles = 2;
		reg.H |= (1 << 6);
		break;
	}
	case 0xF5: {// SET 6,L
		cycles = 2;
		reg.L |= (1 << 6);
		break;
	}
	case 0xF6: {// SET 6,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data |= (1 << 6);
		write(addr, data);
		break;
	}
	case 0xF7: {// SET 6,A
		cycles = 2;
		reg.A |= (1 << 6);
		break;
	}
	case 0xF8: {// SET 7,B
		cycles = 2;
		reg.B |= (1 << 7);
		break;
	}
	case 0xF9: {// SET 7,C
		cycles = 2;
		reg.C |= (1 << 7);
		break;
	}
	case 0xFA: {// SET 7,D
		cycles = 2;
		reg.D |= (1 << 7);
		break;
	}
	case 0xFB: {// SET 7,E
		cycles = 2;
		reg.E |= (1 << 7);
		break;
	}
	case 0xFC: {// SET 7,H
		cycles = 2;
		reg.H |= (1 << 7);
		break;
	}
	case 0xFD: {// SET 7,L
		cycles = 2;
		reg.L |= (1 << 7);
		break;
	}
	case 0xFE: {// SET 7,(HL)
		cycles = 4;
		uint16_t addr = (((uint16_t)reg.H << 8) | reg.L);
		uint8_t data = read(addr);
		data |= (1 << 7);
		write(addr, data);
		break;
	}
	case 0xFF: {// SET 7,A
		cycles = 2;
		reg.A |= (1 << 7);
		break;
	}
	default:
		break;
	}
	return cycles;
}