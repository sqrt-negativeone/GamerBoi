#pragma once
#include <cstdint>
class Bus;
class CPU
{
	//The Registers of the CPU
	uint16_t AF = 0x01b0;
	uint16_t BC = 0x0000;
	uint16_t DE = 0xFF56;
	uint16_t HL = 0x0000;
	uint16_t SP = 0xFFFE;
	uint16_t PC = 0x0100;

	enum flags {
		Z = (1 << 7),
		N = (1 << 6),
		H = (1 << 5),
		C = (1 << 4)
	};

};

