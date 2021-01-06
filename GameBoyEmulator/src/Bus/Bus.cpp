#include "Bus.h"


namespace GamerBoi
{
	Bus::Bus() :
		is_booting(true)
	{

		ppu.connectBus(this);
		cpu.connectBus(this);
		timer.connectBus(this);
		joypad.connectBus(this);

		memset(cpuRam, 0x00, sizeof(cpuRam));
		memset(h_ram, 0x00, sizeof(h_ram));
	}

	Bus::~Bus() {}

	uint8_t Bus::read(uint16_t addr)
	{
		if (is_dma_running && (0xFF00 > addr))
		{// don't read when DMA transfer
			return 0xFF;
		}
		if ((0x0000 <= addr && addr < 0x8000) || (0xA000 <= addr && addr < 0xC000))
		{//read from ROM or external RAM
			if (is_booting && addr == 0x100) is_booting = false;
			if (is_booting && 0x0000 <= addr && addr < 0x0100)
			{// read from boot rom instead
				return boot_rom[addr];
			}
			if (cartridge)
			{
				return cartridge->read(addr);
			}
			else return 0x00;
		}
		if ((0x8000 <= addr && addr < 0xA000) || (0xFE00 <= addr && addr < 0xFEA0))
		{//read from VRAM or OAM tables
			return ppu.read(addr);
		}
		if (0xFF00 <= addr && addr < 0xFF80)
		{//IO registers
			if (addr == 0xFF00)
				return joypad.read();
			if (0xFF04 <= addr && addr <= 0xFF07)
				return timer.read(addr);
			if (addr == 0xFF0F)
				return cpu.read_IF_flag();
			if (addr == 0xFF46)
				return dma_addr;
			if (0xFF40 <= addr && addr <= 0xFF4B)
				return ppu.read_register(addr);
			if (0xFF10 <= addr && addr < 0xFF40)
				return apu.read(addr);
		}
		if (0xFF80 <= addr && addr <= 0xFFFE)
		{//read from HRAM
			return h_ram[addr - 0xFF80];
		}
		if (addr == 0xFFFF)
		{//IE register
			return cpu.read_IE_flag();
		}
		if (0xE000 <= addr && addr < 0xFE00)
		{//mirror of C000-CFFF
			return read(addr - 0x2000);
		}
		if (0xFEA0 <= addr && addr < 0xFF00)
		{// not usable
			return 0xFF;
		}
		if (0xC000 <= addr && addr < 0xE000)
		{//read from RAM
			return cpuRam[addr - 0xC000];
		}
	}
	void Bus::write(uint16_t addr, uint8_t data)
	{
		if (is_dma_running && (0xFF00 > addr))
		{// don't write when DMA transfer
			return;
		}
		//write to ROM or external RAM (writing to rom causes the MBC to change banks)
		if ((0x0000 <= addr && addr < 0x8000) || (0xA000 <= addr && addr < 0xC000))
		{
			if (is_booting) return;
			if (cartridge) cartridge->write(addr, data);
			return;
		}

		if ((0x8000 <= addr && addr < 0xA000) || (0xFE00 <= addr && addr < 0xFEA0))
		{//write to VRAM or OAM tables
			ppu.write(addr, data);
			return;
		}
		if (0xC000 <= addr && addr < 0xE000)
		{// write to RAM
			cpuRam[addr - 0xC000] = data;
		}
		if (addr == 0xFFFF)
		{//IE register
			cpu.write_IE_flag(data);
			return;
		}
		if (0xFF00 <= addr && addr < 0xFF80)
		{//IO registers
			if (addr == 0xFF00)
			{
				joypad.write(data);
				return;
			}
			if (0xFF04 <= addr && addr <= 0xFF07)
			{
				timer.write(addr, data);
				return;
			}
			if (addr == 0xFF0F)
			{
				cpu.write_IF_flag(data);
				return;
			}
			if (addr == 0xFF46)
			{
				dma_addr = data;
				start_dma();
				return;
			}
			if (0xFF40 <= addr && addr <= 0xFF4B)
			{
				ppu.write_register(addr, data);
				return;
			}
			if (0xFF10 <= addr && addr < 0xFF40)
			{
				apu.write(addr, data);
				return;
			}
			return;
		}
		if (0xFF80 <= addr && addr < 0xFFFF)
		{//write to HRAM
			h_ram[addr - 0xFF80] = data;
			return;
		}

		if (0xE000 <= addr && addr < 0xFE00)
		{//mirror of C000-CFFF
			write(addr - 0x2000, data);
			return;
		}

		if (0xFEA0 <= addr && addr < 0xFF00)
		{// not usable
			return;
		}

	}

	void Bus::clock()
	{

		uint8_t cycles = cpu.clock();
		ppu.clock(cycles);
		update_dma(cycles);
		apu.clock(cycles);
		timer.update(cycles);
	}

	void Bus::update_dma(const uint8_t& cycles)
	{
		if (is_dma_running)
		{
			if (dma_remaining_clocks <= cycles)
			{
				dma_remaining_clocks = 0;
				is_dma_running = false;
			}
			else
			{
				dma_remaining_clocks -= cycles;
			}
		}
	}

	void Bus::interrupt_req(uint8_t req)
	{
		uint8_t IF = cpu.read_IF_flag();
		cpu.write_IF_flag(IF | req);
	}

	void Bus::insertCartridge(Cartridge* cartridge)
	{
		if (this->cartridge != nullptr) delete this->cartridge;
		this->cartridge = cartridge;
	}
	void Bus::removeCartridge()
	{
		delete cartridge;
	}
	void Bus::reset()
	{
		cpu.reset();
		ppu.reset();
		timer.reset();
		joypad.reset();
		apu.reset();
		is_dma_running = false;
		is_booting = true;
	}

	void Bus::start_dma()
	{
		uint16_t start_addr = read(0xff46);
		if (start_addr > 0xF1) return;
		start_addr <<= 8;
		for (int i = 0; i < 160; i++)
		{
			ppu.oam[i] = read(start_addr + i);
		}
		is_dma_running = true;
		dma_remaining_clocks = 160;
	}

}
