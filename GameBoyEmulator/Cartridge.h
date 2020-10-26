#pragma once
#include <vector>
class MBC;
class Cartridge
{
public:
	Cartridge(const char* path);
	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);
	std::vector<uint8_t> rom;
	uint8_t *ram;
private:
	MBC* mbc;
};

