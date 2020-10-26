#pragma once
#include <cstdint>
class Bus;
class IORegisters
{
public :
	IORegisters();
	void reset();
	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);
	void connectBus(Bus* bus);
	void inc_LY();
	void inc_DIV();
private:
	uint8_t io_registers[0x80];
	Bus* bus;
};

