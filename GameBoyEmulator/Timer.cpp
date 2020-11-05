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
	}

	void Timer::reset_timer_ticks() {
		timer_ticks = 0;
	}
	uint8_t Timer::get_clockFreq() {
		return freqs[bus->read(TAC) & 0x03];
	}
	void Timer::resetDIV() {
		DIV = 0;
	}

	uint8_t Timer::read_DIV() {
		return (DIV >> 8);
	}
	void Timer::update(uint8_t cycles) {
		DIV += 4 * cycles;
		ticks += 4 * cycles;
		while (ticks >= 16) {
			ticks -= 16;
			//timer ticks happen at 1/16 CPU cycles
			clock();
		}
	}
	void Timer::clock() {
		timer_ticks++;

		if (bus->read(TAC) & 0x04) {//If Timer is on
			auto freq = freqs[bus->read(TAC) & 0x03];

			if (timer_ticks == freq) {
				timer_ticks = 0;
				if (bus->read(TIMA) == 0xFF) {
					bus->write(TIMA, bus->read(TMA));// reset TIMA by the value in TMA
					bus->interrupt_req(1 << 2);
				}
				else {
					bus->write(TIMA, bus->read(TIMA) + 1);
				}
			}
		}
	}
}