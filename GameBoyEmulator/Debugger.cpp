#include "Debugger.h"
#include <string>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
typedef unsigned int UINT;
using std::cin;
using std::string;
Debugger::Debugger() {
	
	
}

void Debugger::start() {	
	for (;;) {
		system("cls");
		showRegisters();
		ShowOptions();
		string ans;
		cin >> ans;
		if (ans == "r") {
			run();
		}
		else if (ans == "rf") {
			runFrame();
		}
		else if (ans == "ri") {
			runInstruction();
		}
		else if (ans == "sv") {
			int s, l;
			cin >> std::hex >> s;
			cin >> std::dec >> l;
			if (s == 0 && l == 0) {
				showVram();
			}
			else {
				showVram(s, l);
			}
		}
		else if (ans == "so") {
			showOAM();
		}
		else if (ans == "bp") {
			cin >> std::hex >> ans;
			int b;
			cin >> std::hex >> b;
			if (ans == "a") {
				setBreakPoint(b);
			}
			else {
				removeBreakPoint(b);
			}
		}
		else if (ans == "sbp") {
			showBreakPoints();
		}
		else if (ans == "reset") {
			reset();
		}
		else if (ans == "sh") {
			showHRam();
		}
		else if (ans == "sm") {
			int addr, l; cin >> std::hex >> addr >> std::dec >> l;
			readMemory(addr, l);
		}
	}
}
void Debugger::setBreakPoint(uint16_t bp) {
	breakpoints.insert(bp);
}
void Debugger::removeBreakPoint(uint16_t bp) {
	if (breakpoints.find(bp) == breakpoints.end()) return;
	breakpoints.erase(bp);
}
void Debugger::run() {
	bool stop = false;
	if (breakpoints.find(bus->cpu.reg.PC) != breakpoints.end()) {
		runInstruction();
	}
	while (!stop) {
		//clock the bus
		if (bus->cpu.cycle == 0 && breakpoints.find(bus->cpu.reg.PC) != breakpoints.end()) {
			break;
		}
		bus->clock();
		//update the stop flag
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
			stop = true;
		}
	}
}
void Debugger::runFrame() {
	UINT current_frame = bus->ppu.frame_count;
	while (current_frame == bus->ppu.frame_count) {
		bus->clock();
	}
}
void Debugger::runInstruction() {
	//if there are some cycles remained from last intsruction
	while (bus->cpu.cycle) {
		bus->clock();
	}
	bus->clock();
	while (bus->cpu.cycle) {
		bus->clock();
	}
}
void Debugger::showOAM() {
	for (int i = 0; i < 160; i += 16) {
		printf("%04X : ", i);
		for (int j = 0; j < 16; j++) {
			printf("%02X ", bus->ppu.oam[i + j]);
		}
		printf("\n");
	}
	_getch();
}
void Debugger::showVram() {
	for (int i = 0; i < 0x2000; i += 16) {
		printf("%04X : ", 0x8000 + i);
		for (int j = 0; j < 16; j++) {
			printf("%02X ", bus->ppu.vram[i + j]);
		}
		printf("\n");
	}
	_getch();
}
void Debugger::showVram(uint16_t start, uint16_t line_count) {
	start -= 0x8000;
	for (int i = start; i < start + 16*line_count; i += 16) {
		printf("%04X : ", 0x8000+i);
		for (int j = 0; j < 16; j++) {
			printf("%02X ", bus->ppu.vram[i + j]);
		}
		printf("\n");
	}
	_getch();
}
void Debugger::showRegisters() {
	printf("AF : %04X, BC : %04X, DE : %04X, HL : %04X, PC : %04X, SP : %04X \nSTAT : %02X, LCDC : %02X\nIME : %01X, IE : %04X, IF : %04X\n",
		bus->cpu.reg.AF.hl, bus->cpu.reg.BC.hl, bus->cpu.reg.DE.hl, bus->cpu.reg.HL.hl, bus->cpu.reg.PC, bus->cpu.reg.SP.hl,
		bus->read(0xFF41), bus->read(0xFF40), bus->cpu.get_IME(), bus->read(0xFFFF), bus->read(0xFF0F));
}
void Debugger::ShowOptions() {
	printf("choose one of the following options\n-reset to reset\n-r to run\n-sbp to show break points\n-rf to run one frame\n-ri to run one instruction\n-so to show OAM\n");
	printf("-sv s l to show VRam, starting from s and with l lines, use sv 0 0 to display all VRam\n");
	printf("-bp a [d] to add breakpoint at address [d]\n-bp r [d] to remove breakpoint at address [d]\n");
	printf("-sh to show HRam\n");
	printf("-sm addr l to show l bytes from memory at address addr\n");
}

void Debugger::showBreakPoints() {
	for (auto x : breakpoints) printf("%04X ", x);
	printf("\n");
	_getch();
}
void Debugger::showHRam() {
	for (int i = 0xDFFF; i >= bus->cpu.reg.SP.hl; i-=2) {
		printf("%04X : %02X%02X\n", i, bus->read(i+1), bus->read(i));
	}
	_getch();
}
void Debugger::reset() {
	bus->reset();
}
void Debugger::readMemory(uint16_t addr,uint16_t nbytes) {
	for (int i = 0; i < nbytes; i++) printf("%04X : %02X\n",addr + i, bus->read(addr + i));
	_getch();
}

void Debugger::getBus(Bus* bus) {
	this->bus = bus;
}
