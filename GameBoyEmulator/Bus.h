#pragma once
#include "CPU.h"
#include "Timer.h"
#include "Cartridge.h"
#include "PPU.h"
#include "Joypad.h"
#include "APU.h"

constexpr auto VBLANK_INTERRUPT = 0b00001;
constexpr auto LCD_STAT_INTERRUPT = 0b00010;
constexpr auto TIMER_INTERRUPT = 0b00100;
constexpr auto SERIAL_INTERRUPT = 0b01000;
constexpr auto JOYPAD_INTERRUPT = 0b10000;

namespace GamerBoi {
	class Bus
	{
	public:
		Bus();
		~Bus();

		uint8_t boot_rom[0x100];// TODO: hard code it (or import it from file)
		uint8_t cpuRam[0x2000]; //8KB Ram
		uint8_t h_ram[0x7F]; //127B High RAM

		uint8_t read(uint16_t addr);
		void write(uint16_t addr, uint8_t data);
	
		void reset();
		bool clock();

		void interrupt_req(uint8_t req);

		void insertCartridge(Cartridge* cartridge);
		void removeCartridge();

		Timer timer;
	
		PPU ppu;
		CPU cpu;
		APU apu;

		Cartridge* cartridge;
		Joypad joypad;
		void start_dma();
		uint8_t dma_remaining_clocks = 0;
	private:
		bool is_booting;	
		bool is_dma_running = false;
		uint8_t dma_addr;
	};

}

