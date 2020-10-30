#include "Debugger.h"

int main() {
	Bus *bus = new Bus();
	//Cartridge* cartridge = new Cartridge("ROMs/Super Mario Land (World).gb");
	//Cartridge* cartridge = new Cartridge("ROMs/Dr. Mario.gb");
	Cartridge* cartridge = new Cartridge("ROMs/Tetris.gb");
	//Cartridge* cartridge = new Cartridge("ROMs/tests/cpu_instrs/cpu_instrs.gb"); //Passed
	//Cartridge* cartridge = new Cartridge("ROMs/tests/instr_timing/instr_timing.gb"); Passed
	//Cartridge* cartridge = new Cartridge("ROMs/tests/mem_timing/mem_timing.gb");
	//Cartridge* cartridge = new Cartridge("ROMs/tests/mem_timing/individual/01-read_timing.gb");
	//Cartridge* cartridge = new Cartridge("ROMs/tests/halt_bug.gb"); // Passed
	//Cartridge* cartridge = new Cartridge("ROMs/tests/cpu_instrs/individual/01-special.gb");
	Debugger db;
	bus->cartridge = cartridge;
	/*db.getBus(bus);
	db.start();*/
	for (;;) {
		bus->clock();
	}
}