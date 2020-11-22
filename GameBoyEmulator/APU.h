#pragma once
#include <cstdint>
#include <vector>
#include "SweepSquareChannel.h"
#include "SquareChannel.h"
#include "WaveChannel.h"
#include "NoiseChannel.h"
#include <functional>
/*
	Sound implementation is based on this resource : https://gbdev.gg8.se/wiki/articles/Gameboy_sound_hardware
*/
namespace GamerBoi {
	class APU
	{
	public:
		APU();
		void clock(uint8_t cycles);
		uint8_t read(uint16_t addr);
		void write(uint16_t addr, uint8_t data);
		void reset();
		
		void set_callback_function(std::function<void(float, float)> callback);
	private:
		SweepSquareChannel square1;
		SquareChannel square2;
		WaveChannel wave;
		NoiseChannel noise;

		struct
		{
			union {
				uint8_t NR0;
				struct {
					uint8_t right_vol : 3;
					uint8_t vin_r_enable : 1;
					uint8_t left_vol : 3;
					uint8_t vin_l_enable : 1;
				};
			};
			union {
				uint8_t NR1;
				struct {
					uint8_t right_enable : 4;
					uint8_t left_enable : 4;
				};
			};
			union {
				uint8_t NR2;
				struct {
					uint8_t channel_length : 4;
					uint8_t _ : 3;
					uint8_t power_control : 1;
				};
			};
		} reg;

		void clock_frame_sequancer();
		void clock_length();
		void clock_volume();
		void mixer(uint8_t sq1, uint8_t sq2, uint8_t wv, uint8_t ns);

		int frame_sequencer_counter;
		uint8_t frame_cycle;
		uint32_t samplerate_counter;

		static constexpr int CLOCKS_PER_512Hz = 2048;
		static constexpr uint32_t CPU_CLOCKS_PER_SIMPLERATE = 1048576 / 44100;

		float output_left;
		float output_rigth;

		//called when there is a sample ready
		std::function<void(float,float)> sampleReady_callback;
	};
}
