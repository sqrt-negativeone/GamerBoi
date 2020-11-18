#pragma once
#include <set>
#include "Bus.h"

namespace GamerBoi {
	class Debugger
	{
	public:
		Debugger();

		void step();
		void setBreakPoint(uint16_t bp);
		void removeBreakPoint(uint16_t bp);
		void run();
		void runInstruction();
		void runFrame();
		void showVram();
		void showVram(uint16_t start, uint16_t line_count);
		void showOAM();
		void showRegisters();
		void ShowOptions();
		void getBus(Bus* bus);
		void showBreakPoints();
		void reset();
		void showHRam();
		void readMemory(uint16_t addr, uint16_t nbytes);
		Bus* bus;
	private:
		std::set<uint16_t> breakpoints;

		char* choices_buff;
		char* vram_buff;
		char* oam_buff;

	};

}

