#pragma once
#include "SquareChannel.h"
namespace GamerBoi {
    class SweepSquareChannel :
        public SquareChannel
    {
    public :
		SweepSquareChannel() :
			SquareChannel(),
			sweep_counter{ 0 },
			frequency_shadow{ 0 },
			sweep_enabled{ false }
		{
			
		}
		uint8_t read(uint8_t reg_num) override {
			if (reg_num == 0) {
				return reg.NR0 | 0x80;
			}
			else {
				return SquareChannel::read(reg_num);
			}
		}
		void write(uint8_t reg_num, uint8_t data) override {
			SquareChannel::write(reg_num, data);
			if (reg_num == 0) {
				sweep_counter = reg.sweep_period;
			}
		}

		void clock_sweep() {
			if (--sweep_counter <= 0) {
				sweep_counter = reg.sweep_period;
				if (sweep_enabled && reg.sweep_period != 0) {
					auto new_freq = frequency_calculation();
					if (new_freq <= 2047 && reg.shift != 0) {
						frequency_shadow = (uint16_t)new_freq;
						reg.frequency_LSB = (new_freq && 0xFF);
						reg.frequency_MSB = (new_freq >> 8);
						frequency_calculation();
					}
				}
			}
		}
		int frequency_calculation() {
			int val = frequency_shadow;
			val >>= reg.shift;
			if (reg.negate) {
				val = frequency_shadow - val;
			}
			else {
				val = frequency_shadow + val;
			}
			is_enable = val < 2047;
			return val;
		}
		void reset() override {
			SquareChannel::reset();
			sweep_counter = 0;
			frequency_shadow = 0;
			sweep_enabled = false;
		}
	private:
		void trigger() override {
			SquareChannel::trigger();

			frequency_shadow = timer;
			sweep_counter = reg.sweep_period;

			sweep_enabled = (reg.sweep_period != 0 || reg.shift != 0);

			if (reg.shift != 0) frequency_calculation();
		}
		int sweep_counter;

		uint16_t frequency_shadow;
		bool sweep_enabled;
    };
}
