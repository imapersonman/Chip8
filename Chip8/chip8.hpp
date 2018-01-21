//
//  chip8.hpp
//  Chip8
//
//  Created by Koissi Adjorlolo on 3/11/16.
//  Copyright © 2016 centuryapps. All rights reserved.
//

#ifndef chip8_hpp
#define chip8_hpp

#include <stdio.h>
#include <stdint.h>
#include "memory.hpp"
#include "keyboard.hpp"
#include "display.hpp"
#include "timer.hpp"

static const uint8_t N_REGISTERS = 0x0010;
static const uint8_t STACK_SIZE = 0x0010;
static const uint8_t SPRITE_SEPARATION = 5;

class Chip8
{
public:
    Chip8();
    // This is what I'll do tomorrow.
    bool loadROM(const char *filePath);
    void execute();
    bool isRunning();
    void dumpRegisters();
private:
    uint8_t getRegister(uint8_t registerIndex);
    uint8_t setRegister(uint8_t registerIndex, uint8_t value);
    void addDefaultSpritesToRAM();
    void clearRegisters();
    void runNextOpcode();
    uint16_t opcodeAtPC();
    
    Memory RAM;
    Keyboard keyboard;
    Display display;
    bool running;
    
    uint8_t registers[N_REGISTERS];
    uint16_t I;
    uint16_t SP;
    uint16_t PC;
    uint16_t stack[STACK_SIZE];
    
    Timer delayTimer;
    Timer soundTimer;
};

#endif /* chip8_hpp */
