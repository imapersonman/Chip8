//
//  memory.cpp
//  Chip8
//
//  Created by Koissi Adjorlolo on 3/11/16.
//  Copyright © 2016 centuryapps. All rights reserved.
//

#include "memory.hpp"

uint8_t Memory::get(uint16_t address)
{
    return (address >= 0 &&
            address < ADDRESS_SPACE) ?
    _ram[address] : 0;
}

// This function could potentially be dangerous.  Not sure yet.
// Returns a pointer to
uint8_t *Memory::getSprite(uint16_t address)
{
    return (address >= 0 &&
            address < ADDRESS_SPACE) ?
    (_ram + address) : nullptr;
}

uint8_t Memory::set(uint16_t address, uint8_t value)
{
    return (address >= 0 &&
            address < ADDRESS_SPACE) ?
    (_ram[address] = value) : 0;
}

void Memory::clear()
{
    for (int address = 0; address < ADDRESS_SPACE; address++)
    {
        set(address, 0);
    }
}
