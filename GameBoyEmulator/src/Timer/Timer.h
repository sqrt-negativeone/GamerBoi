#pragma once
#include <cstdint>
namespace GamerBoi {
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
		uint16_t get_clockFreq();
		void write(uint16_t addr, uint8_t data);
		uint8_t read(uint16_t addr);

	private:

		uint16_t timer_ticks = 0;
		uint16_t divider_ticks = 0;
		Bus* bus;
		uint16_t DIV = 0;
		uint8_t TIMA = 0;
		uint8_t TMA = 0; 
		union {
			uint8_t val;
			struct {
				uint8_t clock_select : 2;
				uint8_t timer_enable : 1;
				uint8_t _ : 5;
			};
		} TAC;

		static constexpr uint16_t freqs[] = {
			1024,16,64,256
		};
	};

}



