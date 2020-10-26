#pragma once
class Bus;
#include <cstdint>
class DMA
{
public :
	bool is_running;
	uint8_t remaining_clocks = 0;
	DMA(Bus *bus);
	void startDMA();
	void clock();
private:
	Bus* bus;
	uint16_t curr_addr;
};

