#pragma once
#include <cstring>
#include "Channel.h"

namespace GamerBoi
{
	class SquareChannel : public Channel
	{
	public:
		SquareChannel() :
			Channel(),
			current_volume{ 0 },
			volume_counter{ 0 },

			waveform_index{ 0 }
		{
			reg.NR0 = 0;
			reg.NR1 = 0;
			reg.NR2 = 0;
			reg.NR3 = 0;
			reg.NR4 = 0;
		}
		virtual uint8_t read(uint8_t reg_num)
		{
			switch (reg_num)
			{
			case 0:
				return reg.NR0 | 0xFF;
			case 1:
				return reg.NR1 | 0x3F;
			case 2:
				return reg.NR2 | 0x00;
			case 3:
				return reg.NR3 | 0xFF;
			case 4:
				return reg.NR4 | 0xBF;
			default:
				break;
			}
		}
		virtual void write(uint8_t reg_num, uint8_t data)
		{
			switch (reg_num)
			{
			case 0:
				reg.NR0 = data;
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
				if (reg.trigger)
				{
					trigger();
				}
				break;

			default:
				break;
			}
		}

		uint8_t clock()
		{
			timer--;
			if (timer <= 0)
			{
				timer = (2048 - get_frequency()) * 4;
				++waveform_index %= 8;
			}
			uint8_t volume = 0;
			if (is_enable && dac_enable)
			{
				volume = current_volume;
			}

			if (!waveform[reg.duty][waveform_index])
			{
				volume = 0;
			}

			return volume;
		}


		void clock_length()
		{
			if (reg.length_enable)
			{
				if (reg.length_load == 0)
				{
					is_enable = false;
				}
				else
				{
					reg.length_load--;
				}
			}
		}
		void clock_volume()
		{
			if (reg.period)
			{
				if (--volume_counter <= 0)
				{
					volume_counter = reg.period;
					int new_volume = (reg.add_mode ? current_volume + 1 : current_volume - 1);
					if (0 <= new_volume && new_volume <= 15)
					{
						current_volume = new_volume;
					}
				}
			}
		}

		void Disable()
		{
			is_enable = false;
			reg.NR0 = 0;
			reg.NR1 = 0;
			reg.NR2 = 0;
			reg.NR3 = 0;
			reg.NR4 = 0;
		}
		virtual void reset()
		{
			timer = 0;
			current_volume = 0;
			volume_counter = 0;
			is_enable = false;
			dac_enable = false;
			waveform_index = 0;
			reg.NR0 = 0;
			reg.NR1 = 0;
			reg.NR2 = 0;
			reg.NR3 = 0;
			reg.NR4 = 0;
		}
	protected:
		virtual void trigger()
		{
			is_enable = true;
			//if (reg.length_load == 0) reg.length_load = 64;
			timer = (2048 - get_frequency()) * 4;
			volume_counter = reg.period;
			current_volume = reg.start_volume;


		}
		uint16_t get_frequency()
		{
			return (uint16_t)(reg.frequency_LSB | (((uint16_t)reg.frequency_MSB) << 8));
		}
		struct
		{
			union
			{
				uint8_t NR0;
				struct
				{
					uint8_t shift : 3;
					uint8_t negate : 1;
					uint8_t sweep_period : 3;
					uint8_t _ : 1;
				};
			};
			union
			{
				uint8_t NR1;
				struct
				{
					uint8_t length_load : 6;
					uint8_t duty : 2;
				};
			};
			union
			{
				uint8_t NR2;
				struct
				{
					uint8_t period : 3;
					uint8_t add_mode : 1;
					uint8_t start_volume : 4;
				};
			};
			union
			{
				uint8_t NR3;
				uint8_t frequency_LSB;
			};
			union
			{
				uint8_t NR4;
				struct
				{
					uint8_t frequency_MSB : 3;
					uint8_t __ : 3;
					uint8_t length_enable : 1;
					uint8_t trigger : 1;
				};
			};
		} reg;

		uint8_t current_volume;
		int volume_counter;

		static constexpr uint8_t waveform[4][8] = {
			{0,0,0,0,0,0,0,1},
			{1,0,0,0,0,0,0,1},
			{1,0,0,0,0,1,1,1},
			{0,1,1,1,1,1,1,0},
		};
		uint8_t waveform_index;
	};
}