#include <cstdint>
#include "Channel.h"
namespace GamerBoi
{
	class WaveChannel : public Channel
	{
	public:
		WaveChannel() :
			Channel(),
			current_volume{ 0 },
			position_counter{ 0 },
			wave_table{ 0 }
		{
			reg.NR0 = 0;
			reg.NR1 = 0;
			reg.NR2 = 0;
			reg.NR3 = 0;
			reg.NR4 = 0;
		}
		uint8_t read_wave_table(uint16_t addr)
		{
			uint8_t table_index = 2 * (addr & 0x000F);
			return (wave_table[table_index] << 4) | wave_table[table_index + 1];
		}
		void write_wave_table(uint16_t addr, uint8_t data)
		{
			uint8_t table_index = 2 * (addr & 0x000F);
			wave_table[table_index] = (data >> 4);
			wave_table[table_index + 1] = (data & 0x0F);
		}

		uint8_t read(uint8_t reg_num)
		{
			switch (reg_num)
			{
			case 0:
				return reg.NR0 | 0x7F;
			case 1:
				return reg.NR1 | 0xFF;
			case 2:
				return reg.NR2 | 0x9F;
			case 3:
				return reg.NR3 | 0xFF;
			case 4:
				return reg.NR4 | 0xBF;
			default:
				break;
			}
		}
		void write(uint8_t reg_num, uint8_t data)
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
			}
		}

		uint8_t clock()
		{
			if (--timer <= 0)
			{
				timer = (2048 - get_frequency()) * 2;
				++position_counter %= 32; //32 size of the wave table
				current_volume = wave_table[position_counter];
				switch (reg.volum_code)
				{
				case 0:
					current_volume >>= 4;
					break;
				case 1:
					current_volume >>= 0;
					break;
				case 2:
					current_volume >>= 1;
					break;
				case 3:
					current_volume >>= 2;
					break;
				}
			}
			if (!is_enable || !reg.dac_power)
			{
				current_volume = 0;
			}
			return current_volume;
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
		void Disable()
		{
			is_enable = false;
			reg.NR0 = 0;
			reg.NR1 = 0;
			reg.NR2 = 0;
			reg.NR3 = 0;
			reg.NR4 = 0;
		}
		void reset()
		{
			timer = 0;
			current_volume = 0;
			position_counter = 0;
			is_enable = false;
			reg.NR0 = 0;
			reg.NR1 = 0;
			reg.NR2 = 0;
			reg.NR3 = 0;
			reg.NR4 = 0;
		}
	private:
		void trigger()
		{
			is_enable = true;
			if (reg.length_load == 0) reg.length_load = 256;
			timer = (2048 - get_frequency()) * 2;
			position_counter = 0;

		}
		uint16_t get_frequency()
		{
			return (uint16_t)(reg.frequency_LSB | ((uint16_t)(reg.frequency_MSB << 8)));
		}
		struct
		{
			union
			{
				uint8_t NR0;
				struct
				{
					uint8_t _ : 7;
					uint8_t dac_power : 1;
				};
			};
			union
			{
				uint8_t NR1;
				uint8_t length_load;
			};
			union
			{
				uint8_t NR2;
				struct
				{
					uint8_t __ : 5;
					uint8_t volum_code : 2;
					uint8_t ___ : 1;
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
					uint8_t ____ : 3;
					uint8_t length_enable : 1;
					uint8_t trigger : 1;
				};
			};
		} reg;

		uint8_t current_volume;
		uint8_t position_counter;

		uint8_t wave_table[0x20];
	};
}