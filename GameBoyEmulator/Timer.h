#pragma once
#include <cstdint>
class Bus;


class Timer
{
public:
	
	void connectBus(Bus* bus);
	void resetDIV();
	uint8_t read_DIV();
	void reset();
	void update(uint8_t cycles);
	void reset_timer_ticks();
	uint8_t get_clockFreq();
private:
	void clock();

	uint16_t ticks = 0;
	uint16_t timer_ticks = 0;
	Bus* bus;
	uint16_t DIV = 0;
	uint16_t TIMA = 0xFF05;
	uint16_t TMA = 0xFF06; 
	uint16_t TAC = 0xFF07;

	static constexpr uint8_t freqs[] = {
		64,1,4,16
	};
};

