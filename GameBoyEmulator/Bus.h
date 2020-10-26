#pragma once
#include <cstdint>
#include <array>

#include "CPU.h"


class Bus
{
public:
	Bus();
	~Bus();

	CPU cpu;
	uint8_t cpuRam[0xFFFF];

	uint8_t read(uint8_t addr);
	void write(uint8_t addr, uint8_t data);
};

