//
//  memory.hpp
//  Chip8
//
//  Created by Koissi Adjorlolo on 3/11/16.
//  Copyright © 2016 centuryapps. All rights reserved.
//

#ifndef memory_hpp
#define memory_hpp

#include <stdio.h>
#include <stdint.h>

static const uint16_t ADDRESS_SPACE = 0x1000;

class Memory
{
public:
    Memory() {}
    // Returns the value located at index 'address'.
    uint8_t get(uint16_t address);
    uint8_t *getSprite(uint16_t address);
    // Sets the value at 'address' to 'value' and returns 'value'.
    uint8_t set(uint16_t address, uint8_t value);
    // Sets all the values to zero.
    void clear();
    
private:
    uint8_t _ram[ADDRESS_SPACE];
    
};

#endif /* memory_hpp */
