//
//  chip8.cpp
//  Chip8
//
//  Created by Koissi Adjorlolo on 3/11/16.
//  Copyright © 2016 centuryapps. All rights reserved.
//

#include <iostream>
#include <math.h>
#include "chip8.hpp"

static const uint16_t RESERVED_START = 0x0000;
static const uint16_t RESERVED_END = 0x01FF;

Chip8::Chip8() : display("Chip8"), running(true)
{
    srand((unsigned int)time(nullptr));
    RAM.clear();
    // Setting up default sprites in memory.
    addDefaultSpritesToRAM();
    clearRegisters();
}

bool Chip8::loadROM(const char *filePath)
{
    FILE *romPointer = fopen(filePath, "rb");
    
    if (romPointer == nullptr)
    {
        std::cout << "Unable to open file '" << filePath << "'." << std::endl;
        return false;
    }
    
    // Size of file.
    fseek(romPointer, 0L, SEEK_END);
    long size = ftell(romPointer);
    fseek(romPointer, 0L, SEEK_SET);
    
    if (size > ADDRESS_SPACE - RESERVED_END - 1)
    {
        std::cout << "ROM size exceeds address space for file '" << filePath << "'." << std::endl;
        return false;
    }
    
    // Cheaty use of getSprite but it gets the job done so cool.
    fread((char *)RAM.getSprite(RESERVED_END + 1), 1, (int)size, romPointer);
    fclose(romPointer);
    
    return true;
}

void Chip8::execute()
{
    // Once set to false, there's no going back.
    running = keyboard.poll() && running;
    runNextOpcode();
    
    // Debug view code should be factored out.  Not good design.
    for (int registerIndex = 0; registerIndex < N_REGISTERS; registerIndex++)
    {
        uint8_t registerValue = getRegister(registerIndex);
        display.setRegisterValue(registerIndex, registerValue);
    }
    display.setIRegisterValue(I);
    display.setStackPointerValue(SP);
    display.setProgramCounterValue(PC);

    display.show();
    delayTimer.update();
    soundTimer.update();
}

bool Chip8::isRunning()
{
    return running;
}

uint8_t Chip8::getRegister(uint8_t registerIndex)
{
    return (registerIndex >= 0 &&
            registerIndex < N_REGISTERS) ?
    registers[registerIndex] : 0;
}

uint8_t Chip8::setRegister(uint8_t registerIndex, uint8_t value)
{
    return (registerIndex >= 0 &&
            registerIndex < N_REGISTERS) ?
    registers[registerIndex] = value : 0;
}

void Chip8::addDefaultSpritesToRAM()
{
    // 0               // 1               // 2               // 3
    RAM.set(0,  0xF0); RAM.set(5,  0x20); RAM.set(10, 0xF0); RAM.set(15, 0xF0);
    RAM.set(1,  0x90); RAM.set(6,  0x60); RAM.set(11, 0x10); RAM.set(16, 0x10);
    RAM.set(2,  0x90); RAM.set(7,  0x20); RAM.set(12, 0xF0); RAM.set(17, 0xF0);
    RAM.set(3,  0x90); RAM.set(8,  0x20); RAM.set(13, 0x80); RAM.set(18, 0x10);
    RAM.set(4,  0xF0); RAM.set(9,  0x70); RAM.set(14, 0xF0); RAM.set(19, 0xF0);
    // 4               // 5               // 6               // 7
    RAM.set(20, 0x90); RAM.set(25, 0xF0); RAM.set(30, 0xF0); RAM.set(35, 0xF0);
    RAM.set(21, 0x90); RAM.set(26, 0x80); RAM.set(31, 0x80); RAM.set(36, 0x10);
    RAM.set(22, 0xF0); RAM.set(27, 0xF0); RAM.set(32, 0xF0); RAM.set(37, 0x20);
    RAM.set(23, 0x10); RAM.set(28, 0x10); RAM.set(33, 0x90); RAM.set(38, 0x40);
    RAM.set(24, 0x10); RAM.set(29, 0xF0); RAM.set(34, 0xF0); RAM.set(39, 0x40);
    // 8               // 9               // A               // B
    RAM.set(40, 0xF0); RAM.set(45, 0xF0); RAM.set(50, 0xF0); RAM.set(55, 0xE0);
    RAM.set(41, 0x90); RAM.set(46, 0x90); RAM.set(51, 0x90); RAM.set(56, 0x90);
    RAM.set(42, 0xF0); RAM.set(47, 0xF0); RAM.set(52, 0xF0); RAM.set(57, 0xE0);
    RAM.set(43, 0x90); RAM.set(48, 0x10); RAM.set(53, 0x90); RAM.set(58, 0x90);
    RAM.set(44, 0xF0); RAM.set(49, 0xF0); RAM.set(54, 0x90); RAM.set(59, 0xE0);
    // C               // D               // E               // F
    RAM.set(60, 0xF0); RAM.set(65, 0xE0); RAM.set(70, 0xF0); RAM.set(75, 0xF0);
    RAM.set(61, 0x80); RAM.set(66, 0x90); RAM.set(71, 0x80); RAM.set(76, 0x80);
    RAM.set(62, 0x80); RAM.set(67, 0x90); RAM.set(72, 0xF0); RAM.set(77, 0xF0);
    RAM.set(63, 0x80); RAM.set(68, 0x90); RAM.set(73, 0x80); RAM.set(78, 0x80);
    RAM.set(64, 0xF0); RAM.set(69, 0xE0); RAM.set(74, 0xF0); RAM.set(79, 0x80);
}

void Chip8::clearRegisters()
{
    for (int registerIndex = 0; registerIndex < N_REGISTERS; registerIndex++)
    {
        registers[registerIndex] = 0;
    }
    
    for (int stackIndex = 0; stackIndex < STACK_SIZE; stackIndex++)
    {
        stack[stackIndex] = 0;
    }
    
    I = 0;
    SP = 0;
    PC = RESERVED_END + 1;
    delayTimer.reset();
    soundTimer.reset();
}

// From: http://stackoverflow.com/questions/199333/how-to-detect-integer-overflow-in-c-c
bool addition_is_safe(uint32_t a, uint32_t b) {
    size_t a_bits = (size_t)log2(a), b_bits = (size_t)log2(b);
    return (a_bits<32 && b_bits<32);
}

void Chip8::runNextOpcode()
{
    uint16_t opcode = opcodeAtPC();
    
    uint16_t nnn = opcode & 0x0FFF;     // Last three nibbles.
    uint16_t kk = opcode & 0x00FF;      // Last two nibbles.
    uint16_t x = (opcode & 0x0F00) >> 8;  // Second nibble.
    uint16_t y = (opcode & 0x00F0) >> 4;  // Third nibble.
    uint16_t n = opcode & 0x000F;       // Last nibble.
    
    //std::cout << std::hex << PC << ": " << std::hex << opcode << std::endl;
    
    switch (opcode & 0xF000)
    {
        case 0x0000:
            if (kk == 0xEE)
            {
                PC = stack[--SP] + 2;
            }
            else if (kk == 0xE0)
            {
                display.clear();
                PC += 2;
            }
            break;
            
        case 0x1000:
            PC = nnn;
            break;
            
        case 0x2000:
            stack[SP++] = PC;
            PC = nnn;
            break;
            
        case 0x3000:
            if (registers[x] == kk)
                PC += 4;
            else
                PC += 2;
            break;
            
        case 0x4000:
            if (registers[x] != kk)
                PC += 4;
            else
                PC += 2;
            break;
            
        case 0x5000:
            if (registers[x] == registers[y])
                PC += 4;
            else
                PC += 2;
            break;
            
        case 0x6000:
            registers[x] = kk;
            PC += 2;
            break;
            
        case 0x7000:
            registers[x] += kk;
            PC += 2;
            break;
            
        case 0x8000:
            switch (n)
            {
                case 0:
                    registers[x] = registers[y];
                    PC += 2;
                    break;
                    
                case 0x1:
                    registers[x] |= registers[y];
                    PC += 2;
                    break;
                    
                case 0x2:
                    registers[x] &= registers[y];
                    PC += 2;
                    break;
                    
                case 0x3:
                    registers[x] ^= registers[y];
                    PC += 2;
                    break;
                    
                case 0x4:
                    if (addition_is_safe(registers[x], registers[y]))
                        registers[0xF] = 1;
                    else
                        registers[0xF] = 0;
                    registers[x] += registers[y];
                    PC += 2;
                    break;
                    
                case 0x5:
                    if (registers[x] > registers[y])
                        registers[0xF] = 1;
                    else
                        registers[0xF] = 0;
                    registers[x] -= registers[y];
                    PC += 2;
                    break;
                    
                case 0x6:
                    if ((registers[x] & -registers[x]) == 1)
                        registers[0xF] = 1;
                    else
                        registers[0xF] = 0;
                    registers[x] = registers[x] >> 1;
                    PC += 2;
                    break;
                    
                case 0x7:
                    if (registers[y] > registers[x])
                        registers[0xF] = 1;
                    else
                        registers[0xF] = 0;
                    registers[x] = registers[y] - registers[x];
                    PC += 2;
                    break;
                    
                case 0xE:
                    if (registers[x] & 0x80)
                        registers[0xF] = 1;
                    else
                        registers[0xF] = 0;
                    registers[x] = registers[x] << 1;
                    PC += 2;
                    break;
                    
                default:
                    std::cout << "Unrecognized opcode '" << std::hex << opcode << "'." << std::endl;
                    running = false;
                    break;
            }
            break;
            
        case 0x9000:
            if (registers[x] != registers[y])
                PC += 4;
            else
                PC += 2;
            break;
            
        case 0xA000:
            I = nnn;
            PC += 2;
            break;
            
        case 0xB000:
            PC = nnn + registers[0];
            break;
            
        case 0xC000:
            registers[x] = (rand() % 256) & kk;
            PC += 2;
            break;
            
        case 0xD000:
            if (display.setSprite(registers[x], registers[y], RAM.getSprite(I), n))
                registers[0xF] = 1;
            else
                registers[0xF] = 0;
            PC += 2;
            break;
            
        case 0xE000:
            switch (kk)
            {
                case 0x9E:
                    if (keyboard.isKeyPressed(registers[x]))
                        PC += 4;
                    else
                        PC += 2;
                    break;
                    
                case 0xA1:
                    if (!keyboard.isKeyPressed(registers[x]))
                        PC += 4;
                    else
                        PC += 2;
                    break;
                    
                default:
                    std::cout << "Unrecognized opcode '" << std::hex << opcode << "'." << std::endl;
                    running = false;
                    break;
            }
            break;
            
        case 0xF000:
            switch (kk)
            {
                case 0x07:
                    registers[x] = delayTimer.get();
                    PC += 2;
                    break;
                    
                case 0x0A:
                    registers[x] = keyboard.waitForKeyPress();
                    if (registers[x] == UINT8_MAX)
                        exit(0);
                    PC += 2;
                    break;
                    
                case 0x15:
                    delayTimer.set(registers[x]);
                    PC += 2;
                    break;
                    
                case 0x18:
                    soundTimer.set(registers[x]);
                    PC += 2;
                    break;
                    
                case 0x1E:
                    I += registers[x];
                    PC += 2;
                    break;
                    
                case 0x29:
                    I = RESERVED_START + SPRITE_SEPARATION * registers[x];
                    PC += 2;
                    break;
                    
                case 0x33:
                    RAM.set(I, registers[x] / 100 % 10);
                    RAM.set(I + 1, registers[x] / 10 % 10);
                    RAM.set(I + 2, registers[x] % 10);
                    PC += 2;
                    break;
                    
                case 0x55:
                    for (int regIndex = 0; regIndex < N_REGISTERS; regIndex++)
                    {
                        RAM.set(I + regIndex, registers[regIndex]);
                    }
                    PC += 2;
                    break;
                    
                case 0x65:
                    for (int regIndex = 0; regIndex < N_REGISTERS; regIndex++)
                    {
                        registers[x] = RAM.get(I + regIndex);
                    }
                    PC += 2;
                    break;
                    
                default:
                    std::cout << "Unrecognized opcode '" << std::hex << opcode << "'." << std::endl;
                    running = false;
                    break;
            }
            break;
            
        default:
            std::cout << "Unrecognized opcode '" << std::hex << opcode << "'." << std::endl;
            running = false;
            break;
    }
    
    if (SP < 0)
    {
        std::cout << "Stack underflow." << std::endl;
        running = false;
    }
    else if (SP >= STACK_SIZE)
    {
        std::cout << "Stack overflow." << std::endl;
        running = false;
    }
    
    //dumpRegisters();
}

uint16_t Chip8::opcodeAtPC()
{
    uint16_t opcode = RAM.get(PC) << 8 |
                      RAM.get(PC + 1);
    
    return opcode;
}

void Chip8::dumpRegisters()
{
    for (int regIndex = 0; regIndex < N_REGISTERS; regIndex++)
    {
        std::cout << "v" << std::hex << (uint16_t)regIndex << ": " << registers[regIndex] << std::endl;
    }
    
    std::cout << "SP: " << std::hex << (uint16_t)SP << std::endl;
    std::cout << "PC: " << std::hex << (uint16_t)PC << std::endl;
    std::cout << "I: " << std::hex << (uint16_t)I << std::endl;
    
    std::cout << std::endl;
}
