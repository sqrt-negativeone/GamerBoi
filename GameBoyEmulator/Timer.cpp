#include "Timer.h"
#include "Bus.h"
namespace GamerBoi {

	void Timer::connectBus(Bus* bus) {
		this->bus = bus;
	}

	void Timer::reset() {
		//DIV = 0xABCC;
		DIV = 0;
		timer_ticks = 0;
		TIMA = 0;
		TMA = 0;
		TAC.val = 0;
		TAC._ = 0x1F;
	}

	void Timer::reset_timer_ticks() {
		timer_ticks = 0;
	}
	uint16_t Timer::get_clockFreq() {
		return freqs[TAC.clock_select];
	}
	void Timer::resetDIV() {
		DIV = 0;
		timer_ticks = 0;
	}

	void Timer::write(uint16_t addr, uint8_t data) {
		switch (addr)
		{
		case 0xFF04: //DIV
			resetDIV();
			break;
		case 0xFF05: //TIMA
			TIMA = data;
			break;
		case 0xFF06: //TMA
			TMA = data;
			break;
		case 0xFF07: //TAC
			auto old_freq = get_clockFreq();
			TAC.val = data;
			TAC._ = 0x1F;
			if (get_clockFreq() != old_freq) {
				reset_timer_ticks();
			}
		}
	}

	uint8_t Timer::read(uint16_t addr) {
		switch (addr)
		{
		case 0xFF04: //DIV
			return read_DIV();
		case 0xFF05: //TIMA
			return TIMA;
		case 0xFF06: //TMA
			return TMA;
		case 0xFF07: //TAC
			return TAC.val;
		}
	}
	

	uint8_t Timer::read_DIV() {
		return (DIV >> 8);
	}
	void Timer::update(uint8_t cycles) {
		uint8_t t_cycles = 4 * cycles;
		DIV += t_cycles;
		if (TAC.timer_enable) {
			timer_ticks += t_cycles;
			auto freq = get_clockFreq();
			while (timer_ticks >= freq) {
				timer_ticks -= freq;
				if (TIMA == 0xFF) {
					TIMA = TMA;// reset TIMA by the value in TMA
					bus->interrupt_req(TIMER_INTERRUPT);
				}
				else {
					TIMA++;
				}
			}
		}
		
	}
	
}