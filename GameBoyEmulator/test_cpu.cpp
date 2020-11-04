#include "Debugger.h"

int main() {
	Bus *bus = new Bus();
	Cartridge* cartridge = new Cartridge("ROMs/Super Mario Land (World).gb");
	//Cartridge* cartridge = new Cartridge("ROMs/Dr. Mario.gb");
	//Cartridge* cartridge = new Cartridge("ROMs/Tetris.gb");
	Debugger db;
	bus->cartridge = cartridge;
	/*db.getBus(bus);
	db.start();*/
	for (;;) {
		bus->clock();
	}
}