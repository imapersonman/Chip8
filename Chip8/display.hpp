//
//  display.hpp
//  Chip8
//
//  Created by Koissi Adjorlolo on 3/11/16.
//  Copyright © 2016 centuryapps. All rights reserved.
//

#ifndef display_hpp
#define display_hpp

#include <stdio.h>
#include <stdint.h>

static const uint8_t SPRITE_WIDTH = 8;
static const uint8_t SPRITE_HEIGHT = 5;
// Bytes per sprite.
static const uint8_t REALPIXELS_PER_PIXEL = 10;
// No magic numbers!!!
static const uint8_t BITS_PER_BYTE = 8;

static const uint8_t SCREEN_WIDTH = 64;
static const uint8_t SCREEN_HEIGHT = 32;

static const uint16_t MEMORY_SIZE = (SCREEN_WIDTH / BITS_PER_BYTE) * SCREEN_HEIGHT;

// Get rid of later.
static const uint8_t N_REGS = 0x0010;

class Display
{
public:
    Display(const char *title);
    
    void clear();
    bool get(uint8_t xPixel, uint8_t yPixel);
    // True for white, false for black.
    void set(uint8_t xPixel, uint8_t yPixel, bool value);
    // Returns true if sprites overlap, false otherwise.
    bool setSprite(uint8_t xPixel, uint8_t yPixel, uint8_t *sprite, uint8_t spriteHeight);
    void show();

    // For debugger
    void setRegisterValue(int index, uint8_t value);
    void setIRegisterValue(uint16_t value);
    void setStackPointerValue(uint16_t value);
    void setProgramCounterValue(uint16_t value);
private:
    uint8_t _memory[MEMORY_SIZE];
    // For debugger
    uint8_t _registers[N_REGS];
    uint16_t _I;
    uint16_t _SP;
    uint16_t _PC;
};

#endif /* display_hpp */
