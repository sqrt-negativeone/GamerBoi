#include "APU.h"

namespace GamerBoi
{
	APU::APU() :
		frame_sequencer_counter{ CLOCKS_PER_512Hz },
		samplerate_counter{ 0 },
		output_left(0.f),
		output_rigth(0.f),
		frame_cycle(0)
	{
		reg.NR0 = 0x77;
		reg.NR1 = 0xF3;
		reg.NR2 = 0xF1;
	}
	void APU::reset()
	{
		frame_sequencer_counter = CLOCKS_PER_512Hz;
		frame_cycle = 0;
		samplerate_counter = 0;
		reg.NR0 = 0;
		reg.NR1 = 0;
		reg.NR2 = 0;
		square1.reset();
		square2.reset();
		wave.reset();
		noise.reset();
	}
	void APU::clock(uint8_t cycles)
	{
		//this may cause a bug if the power control is off and the audio device is waiting for sample data, it may wait endlessly
		if (reg.power_control == 0)
		{
			return;
		}
		while (cycles--)
		{
			if (frame_sequencer_counter-- == 0)
			{
				frame_sequencer_counter = CLOCKS_PER_512Hz;

				clock_frame_sequancer();
			}
			uint8_t sq1 = square1.clock();
			uint8_t sq2 = square2.clock();
			uint8_t wv = wave.clock();
			uint8_t ns = noise.clock();

			if (samplerate_counter-- == 0)
			{
				//sample is ready
				samplerate_counter = CPU_CLOCKS_PER_SAMPLERATE;
				mixer(sq1, sq2, wv, ns);
				if (sampleReady_callback) sampleReady_callback(output_left, output_rigth);
			}
		}
	}
	uint8_t APU::read(uint16_t addr)
	{
		//square 1
		if (0xFF10 <= addr && addr < 0xFF15)
		{
			return square1.read(addr - 0xFF10);
		}
		//square 2
		if (0xFF15 <= addr && addr < 0xFF1A)
		{
			return square2.read(addr - 0xFF15);
		}
		//wave
		if (0xFF1A <= addr && addr < 0xFF1F)
		{
			return wave.read(addr - 0xFF1A);
		}
		//noise
		if (0xFF1F <= addr && addr < 0xFF24)
		{
			return noise.read(addr - 0xFF1F);
		}
		//wave patter
		if (0xFF30 <= addr && addr < 0xFF40)
		{
			return wave.read_wave_table(addr);
		}
		if (addr == 0xFF24)
		{
			return reg.NR0;
		}
		if (addr == 0xFF25)
		{
			return reg.NR1;
		}
		if (addr == 0xFF26)
		{
			return (reg.NR2 | (square1.Is_enable() << 0) | (square2.Is_enable() << 1) | (wave.Is_enable() << 2) | (noise.Is_enable() << 3)) | 0x70;
		}
		return 0xFF;
	}
	void APU::write(uint16_t addr, uint8_t data)
	{
		if (addr == 0xFF26)
		{
			if ((data & 0x80) == 0)
			{
				square1.Disable();
				square2.Disable();
				wave.Disable();
				noise.Disable();

				reg.NR0 = 0;
				reg.NR1 = 0;

				frame_cycle = 0;
			}
			if (!reg.power_control && (data & 0x80))
			{
				frame_sequencer_counter = CLOCKS_PER_512Hz;
			}
			reg.NR2 = (data & 0x80);
		}
		//wave patter
		if (0xFF30 <= addr && addr < 0xFF40)
		{
			wave.write_wave_table(addr, data);
		}
		if (!reg.power_control)
		{
			return;
		}
		if (0xFF10 <= addr && addr < 0xFF15)
		{
			square1.write(addr - 0xFF10, data);
		}
		//square 2
		if (0xFF15 <= addr && addr < 0xFF1A)
		{
			square2.write(addr - 0xFF15, data);
		}
		//wave
		if (0xFF1A <= addr && addr < 0xFF1F)
		{
			wave.write(addr - 0xFF1A, data);
		}
		//noise
		if (0xFF1F <= addr && addr < 0xFF24)
		{
			noise.write(addr - 0xFF1F, data);
		}

		if (addr == 0xFF24)
		{
			reg.NR0 = data;
		}
		if (addr == 0xFF25)
		{
			reg.NR1 = data;
		}

	}

	void APU::clock_frame_sequancer()
	{
		if (frame_cycle == 0 || frame_cycle == 2 || frame_cycle == 4 || frame_cycle == 6)
		{
			clock_length();
		}
		if (frame_cycle == 2 || frame_cycle == 6)
		{
			square1.clock_sweep();
		}
		if (frame_cycle == 7)
		{
			clock_volume();
		}

		++frame_cycle &= 8;
	}
	void APU::clock_length()
	{
		square1.clock_length();
		square2.clock_length();
		wave.clock_length();
		noise.clock_length();
	}
	void APU::clock_volume()
	{
		square1.clock_volume();
		square2.clock_volume();
		noise.clock_volume();
	}
	void APU::mixer(uint8_t sq1, uint8_t sq2, uint8_t wv, uint8_t ns)
	{
		//all sounds are less than 0xF
		auto s1 = sq1 / 15.f;
		auto s2 = sq2 / 15.f;
		auto s3 = wv / 15.f;
		auto s4 = ns / 15.f;

		float left = 0;
		float right = 0;

		if (reg.right_enable & 0b0001)
		{
			right += s1;
		}
		if (reg.right_enable & 0b0010)
		{
			right += s2;
		}
		if (reg.right_enable & 0b0100)
		{
			right += s3;
		}
		if (reg.right_enable & 0b1000)
		{
			right += s4;
		}

		if (reg.left_enable & 0b0001)
		{
			left += s1;
		}
		if (reg.left_enable & 0b0010)
		{
			left += s2;
		}
		if (reg.left_enable & 0b0100)
		{
			left += s3;
		}
		if (reg.left_enable & 0b1000)
		{
			left += s4;
		}

		left /= 4.f;
		right /= 4.f;

		auto left_vol = reg.left_vol / 7.f;
		auto right_vol = reg.right_vol / 7.f;

		left = (left * left_vol);
		right = (right * right_vol);

		output_left = left;
		output_rigth = right;
	}

	void APU::set_callback_function(std::function<void(float, float)> callback)
	{
		sampleReady_callback = callback;
	}
}