#pragma once
#include "CPU.h"
#include "Timer.h"
#include "Cartridge.h"
#include "PPU.h"
#include "IORegisters.h"
#include "Joypad.h"

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

		Timer timer;
	
		PPU ppu;
		CPU cpu;
		Cartridge* cartridge;
		IORegisters io_registers;
		Joypad joypad;
		void start_dma();
		uint8_t dma_remaining_clocks = 0;
	private:
		bool is_booting;	
		bool is_dma_running = false;
	
	};

}

