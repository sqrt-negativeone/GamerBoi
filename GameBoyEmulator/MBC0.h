#pragma once
#include "MBC.h"
namespace GamerBoi {
    class MBC0 :
        public MBC
    {
    public :
        MBC0(Cartridge *cart);
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);
    };

}
