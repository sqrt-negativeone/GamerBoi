#pragma once
#include <cstdint>

namespace GamerBoi {
	class Bus;
	class CPU
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
		//execute one instruction
		uint8_t clock();
		void connectBus(Bus* b);

		//external signals
		void reset();
		void handle_intr();
		uint8_t cycle = 0; // in M-cycle

		uint8_t read_IE_flag();
		void write_IE_flag(uint8_t data);
		uint8_t read_IF_flag();
		void write_IF_flag(uint8_t data);

		bool get_IME() {
			return this->IME;
		}
	private:
		bool IME; //  Interrupt Master Enable Flag 
		bool is_halt;
		bool is_stop;

		union {
			uint8_t val;
			struct {
				uint8_t vblank : 1;
				uint8_t lcd_stat : 1;
				uint8_t timer : 1;
				uint8_t serial : 1;
				uint8_t joypad : 1;
				uint8_t _ : 3;
			};
		} IE, IF;
		uint8_t ime_enable_waiting = -1;
		uint8_t ime_disable_waiting = -1;
		Bus* bus = nullptr;

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

		void (CPU::*alu[8])(uint8_t data) = { &CPU::ADD,&CPU::ADC,&CPU::SUB,&CPU::SBC,&CPU::AND,&CPU::XOR,&CPU::OR,&CPU::CP };
		void (CPU::* rot[8])(uint8_t* data) = { &CPU::RLC,&CPU::RRC,&CPU::RL,&CPU::RR,&CPU::SLA,&CPU::SRA,&CPU::SWAP,&CPU::SRL };
		bool (CPU::* cc[4])() = {&CPU::NZ,&CPU::Z,&CPU::NC,&CPU::C};

		void ADD(uint8_t data); void ADC(uint8_t data); void SUB(uint8_t data); void SBC(uint8_t data);
		void AND(uint8_t data); void OR(uint8_t data); void XOR(uint8_t data); void CP(uint8_t data);

		void RLC(uint8_t* data); void RRC(uint8_t* data); void RL(uint8_t* data); void RR(uint8_t* data);
		void SLA(uint8_t* data); void SRA(uint8_t* data); void SWAP(uint8_t* data); void SRL(uint8_t* data);

		bool NZ(), Z(), NC(), C();
	};

}

