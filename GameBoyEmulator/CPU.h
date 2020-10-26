#pragma once
#include <cstdint>
#include <string>
class Bus;
class CPU
{
public:
	//The Registers of the CPU
	struct Registers {
		uint8_t A = 0x00;
		uint8_t B = 0x00;
		uint8_t C = 0x00;
		uint8_t D = 0x00;
		uint8_t E = 0x00;
		uint8_t F = 0x00;
		uint8_t H = 0x00;
		uint8_t L = 0x00;
		uint16_t SP = 0;
		uint16_t PC = 0;
	} reg;
	

	//perform one clock cycle
	void clock();
	void connectBus(Bus *b);

	//external signals
	void reset();
	void handle_intr();

	enum flags {
		Z = (1 << 7),
		N = (1 << 6),
		H = (1 << 5),
		C = (1 << 4)
	};
	//access flags
	uint8_t getFlag(flags f);
	void setFlag(flags f, bool v);
private:
	bool IME; //  Interrupt Master Enable Flag 
	bool is_halt;
	bool is_stop;

	Bus* bus = nullptr;
	
	uint8_t cycle = 0; // in M-cycle
	uint32_t clock_count = 0;

	

	

	//read and write to/from memory
	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);

	//OPCODES 
	uint8_t exec_opcode(uint8_t opcode);
	uint8_t exec_prefixed(uint8_t opcode);
	
};

/*
	ADDRMOD: (from where to get data)
	IMP	: implied no data needed
	IMM : just after the the opcode

	INSTRUCTIONS:
	NOP		LD		INC		DEC		RLCA	ADD		RRCA	
	STOP	RLA		JR		RRA		DAA		CCF		HALT
	SUB		SBC		AND		XOR		OR		CP		RET
	POP		JP		CALL	PUSH	RST		PREFIX	ADC
	RETI	LDH		DI		EI		
	
	PREFIXED
	RLC		RRC		RL		RR		SLA		SRA		SWAP
	SRL		BIT		RES		SET		
*/