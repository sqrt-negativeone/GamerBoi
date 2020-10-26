#pragma once
#include <cstdint>
class Bus;
class CPU2
{
	//The Registers of the CPU
	//PS: this is working on a little endian machine
	//TODO: make it endian free
public:
	struct Registers {
		union {
			struct {
				uint8_t l,h;
			};
			uint16_t hl;
		} BC,DE,HL,SP;

		union {
			uint16_t hl;
			struct {
				union {
					uint8_t l;
					struct {
						uint8_t unused : 4;
						uint8_t C : 1, H : 1, N : 1, Z : 1;
					};
				};
				uint8_t h;
			};
		} AF;
		uint16_t PC;
	} reg;

	union {
		uint8_t opcode;
		struct {
			uint8_t z : 3;
			uint8_t y : 3;
			uint8_t x : 2;
		};
	} op;
	//perform one clock cycle
	void clock();
	void connectBus(Bus* b);

	//external signals
	void reset();
	void handle_intr();
	uint8_t cycle = 0; // in M-cycle

	uint8_t read_IE_flag();
	void write_IE_flag(uint8_t data);
	bool get_IME() {
		return this->IME;
	}
private:
	bool IME; //  Interrupt Master Enable Flag 
	bool is_halt;
	bool is_stop;
	uint8_t IE;
	uint8_t ime_enable_waiting = -1;
	uint8_t ime_disable_waiting = -1;
	Bus* bus = nullptr;

	uint32_t clock_count = 0;

	bool halt_bug = false;



	//read and write to/from memory
	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);

	//OPCODES 
	uint8_t exec_opcode();
	uint8_t exec_prefixed();

	uint8_t* r[8] = { &reg.BC.h , &reg.BC.l, &reg.DE.h, &reg.DE.l, &reg.HL.h, &reg.HL.l, nullptr, &reg.AF.h };
	uint16_t* rp[4] = { &reg.BC.hl,&reg.DE.hl,&reg.HL.hl,&reg.SP.hl };
	uint16_t* rp2[4] = { &reg.BC.hl,&reg.DE.hl,&reg.HL.hl,&reg.AF.hl };

	void (CPU2::*alu[8])(uint8_t data) = { &CPU2::ADD,&CPU2::ADC,&CPU2::SUB,&CPU2::SBC,&CPU2::AND,&CPU2::XOR,&CPU2::OR,&CPU2::CP };
	void (CPU2::* rot[8])(uint8_t* data) = { &CPU2::RLC,&CPU2::RRC,&CPU2::RL,&CPU2::RR,&CPU2::SLA,&CPU2::SRA,&CPU2::SWAP,&CPU2::SRL };
	bool (CPU2::* cc[4])() = {&CPU2::NZ,&CPU2::Z,&CPU2::NC,&CPU2::C};

	void ADD(uint8_t data); void ADC(uint8_t data); void SUB(uint8_t data); void SBC(uint8_t data);
	void AND(uint8_t data); void OR(uint8_t data); void XOR(uint8_t data); void CP(uint8_t data);

	void RLC(uint8_t* data); void RRC(uint8_t* data); void RL(uint8_t* data); void RR(uint8_t* data);
	void SLA(uint8_t* data); void SRA(uint8_t* data); void SWAP(uint8_t* data); void SRL(uint8_t* data);

	bool NZ(), Z(), NC(), C();
};

