#include <cstdint>
#include "Channel.h"
namespace GamerBoi {
	class NoiseChannel : public Channel {
	public:
		NoiseChannel() :
			Channel(),
			current_volume{ 0 },
			lfsr_register{ 0 },
			volume_counter{ 0 },
			output{ 0 }
		{
			reg.NR0 = 0;
			reg.NR1 = 0;
			reg.NR2 = 0;
			reg.NR3 = 0;
			reg.NR4 = 0;
		}
		uint8_t read(uint8_t reg_num) {
			switch (reg_num)
			{
			case 0:
				return reg.NR0 | 0xFF;
			case 1:
				return reg.NR1 | 0xFF;
			case 2:
				return reg.NR2 | 0x00;
			case 3:
				return reg.NR3 | 0x00;
			case 4:
				return reg.NR4 | 0xBF;
			}
		}
		void write(uint8_t reg_num, uint8_t data) {
			switch (reg_num)
			{
			case 0:
				break;
			case 1:
				reg.NR1 = data;
				break;
			case 2:
				reg.NR2 = data;
				dac_enable = (reg.start_volume || reg.add_mode);
				volume_counter = reg.period;
				current_volume = reg.start_volume;
				break;
			case 3:
				reg.NR3 = data;
				break;
			case 4:
				reg.NR4 = data;
				if (reg.trigger) {
					trigger();
				}
				break;

			}
		}

		uint8_t clock() {
			if (timer-- == 0) {
				set_timer();
				uint16_t xor_result = ((lfsr_register & 1) ^ ((lfsr_register & 2) >> 1));
				lfsr_register >>= 1;
				lfsr_register |= (xor_result << 14);
				if (reg.lfsr_mode) {
					lfsr_register |= (xor_result << 6);
					lfsr_register &= 0x7F;
				}
				output = current_volume * ((is_enable && dac_enable && !(lfsr_register & 1)) == 1);
			}
			return output;
		}

		void clock_length() {
			if (reg.length_enable && reg.length_load > 0) {
				if (--reg.length_load == 0) {
					is_enable = false;
				}
			}
		}
		void clock_volume() {
			if (--volume_counter <= 0) {
				volume_counter = reg.period;
				if (reg.period != 0) {
					int new_volume = reg.add_mode ? current_volume + 1 : current_volume - 1;
					if (0 <= new_volume && new_volume <= 15) {
						current_volume = new_volume;
					}
				}
			}
		}
		void Disable() {
			is_enable = false;
			reg.NR0 = 0;
			reg.NR1 = 0;
			reg.NR2 = 0;
			reg.NR3 = 0;
			reg.NR4 = 0;
		}
		void reset() {
			volume_counter = 0;
			output = 0;
			is_enable = false;
			dac_enable = false;
			timer = 0;
			current_volume = 0;
			lfsr_register = 0;
			reg.NR0 = 0;
			reg.NR1 = 0;
			reg.NR2 = 0;
			reg.NR3 = 0;
			reg.NR4 = 0;
		}
	private:
		void trigger() {
			is_enable = true;
			if (reg.length_load == 0) reg.length_load = 64;
			set_timer();
			lfsr_register = 0xFFFF;
			volume_counter = reg.period;
			current_volume = reg.start_volume;
		}
		void set_timer() {
			uint16_t base = 0;
			switch (reg.divisor_code)
			{
			case 0:
				base = 8;
				break;
			case 1:
				base = 16;
				break;
			case 2:
				base = 32;
				break;
			case 3:
				base = 48;
				break;
			case 4:
				base = 64;
				break;
			case 5:
				base = 80;
				break;
			case 6:
				base = 96;
				break;
			case 7:
				base = 112;
				break;
			}
			timer = (base << reg.clock_shift);
		}
		struct {
			union
			{
				uint8_t NR0;
				uint8_t _;
			};
			union
			{
				uint8_t NR1;
				struct {
					uint8_t length_load : 6;
					uint8_t __ : 2;
				};
			};
			union
			{
				uint8_t NR2;
				struct {
					uint8_t period : 3;
					uint8_t add_mode : 1;
					uint8_t start_volume : 4;
				};
			};
			union {
				uint8_t NR3;
				struct
				{
					uint8_t divisor_code : 3;
					uint8_t lfsr_mode : 1;
					uint8_t clock_shift : 4;
				};
			};
			union {
				uint8_t NR4;
				struct {
					uint8_t ___ : 6;
					uint8_t length_enable : 1;
					uint8_t trigger : 1;
				};
			};
		} reg;
		uint8_t current_volume;
		uint16_t lfsr_register;
		int volume_counter;
		uint8_t output;
	};
}