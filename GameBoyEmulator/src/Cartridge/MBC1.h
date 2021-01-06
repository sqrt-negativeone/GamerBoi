#pragma once
#include "MBC.h"
namespace GamerBoi {
    class MBC1 :
        public MBC
    {
    public:
        MBC1(Cartridge *cart);
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);
    private:
        bool mode;
    };

}

