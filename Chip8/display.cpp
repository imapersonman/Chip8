//
//  display.cpp
//  Chip8
//
//  Created by Koissi Adjorlolo on 3/11/16.
//  Copyright © 2016 centuryapps. All rights reserved.
//

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <sstream>
#include "display.hpp"
#include "font-cache.h"

static SDL_Window *window;
static SDL_Renderer *renderer;

static SDL_Window *debugWindow;
static SDL_Renderer *debugRenderer;

static int DEBUG_WINDOW_WIDTH = 200;
static int DEBUG_WINDOW_HEIGHT = 600;

static FontCache::font debugFont;

Display::Display(const char *title)
{
    clear();
    
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "Unable to initialize video.  Exiting.";
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    
    
    window = SDL_CreateWindow(title,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH * REALPIXELS_PER_PIXEL,
                              SCREEN_HEIGHT * REALPIXELS_PER_PIXEL,
                              0);
    
    if (window == nullptr)
    {
        std::cout << "Unable to create window." << std::endl;
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    
    renderer = SDL_CreateRenderer(window, -1, 0);
    
    if (renderer == nullptr)
    {
        std::cout << "Unable to create renderer." << std::endl;
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    debugWindow = SDL_CreateWindow("Debugger",
        0,
        SDL_WINDOWPOS_UNDEFINED,
        DEBUG_WINDOW_WIDTH,
        DEBUG_WINDOW_HEIGHT,
        0);

    if (debugWindow == nullptr)
    {
        std::cout << "Unable to create debug window." << std::endl;
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    debugRenderer = SDL_CreateRenderer(debugWindow, -1, 0);

    if (renderer == nullptr)
    {
        std::cout << "Unable to create debug renderer." << std::endl;
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    if (TTF_Init() == -1)
    {
        std::cout << "Unable to initialize SDL_TTF." << std::endl;
        std::cout << TTF_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    
    debugFont = FontCache::LoadFont(debugRenderer, "Fonts/Andale Mono.ttf", 16);
}

void Display::clear()
{
    for (int pixelIndex = 0; pixelIndex < MEMORY_SIZE; pixelIndex++)
    {
        _memory[pixelIndex] = 0;
    }
}

bool Display::get(uint8_t xPixel, uint8_t yPixel)
{
    uint8_t xBitIndex = xPixel % 8;
    uint8_t xByteIndex = xPixel / BITS_PER_BYTE;
    uint8_t yByteIndex = yPixel;
    uint8_t absoluteIndex = yByteIndex * (SCREEN_WIDTH / 8) + xByteIndex;
    uint8_t byteToModify = _memory[absoluteIndex];
    
    return byteToModify & (0x1 << (BITS_PER_BYTE - 1 - xBitIndex));
}

void Display::set(uint8_t xPixel, uint8_t yPixel, bool value)
{
    if (xPixel >= 0 && xPixel < SCREEN_WIDTH &&
        yPixel >= 0 && yPixel < SCREEN_HEIGHT)
    {
        uint8_t xBitIndex = xPixel % BITS_PER_BYTE;
        uint8_t xByteIndex = xPixel / BITS_PER_BYTE;
        uint8_t yByteIndex = yPixel;
        uint8_t absoluteIndex = yByteIndex * (SCREEN_WIDTH / 8) + xByteIndex;
        
        if (!value)
        {
            _memory[absoluteIndex] &= (value << (BITS_PER_BYTE - 1 - xBitIndex));
        }
        
        _memory[absoluteIndex] ^= value << (BITS_PER_BYTE - 1 - xBitIndex);
    }
}

bool Display::setSprite(uint8_t xPixel, uint8_t yPixel, uint8_t *sprite, uint8_t spriteHeight)
{
    bool overlaps = false;
    
    // No early exit for function in case a part of the sprite is on screen.
    for (int ySpriteIndex = 0; ySpriteIndex < spriteHeight; ySpriteIndex++)
    {
        uint8_t yByteIndex = yPixel + ySpriteIndex;
        // Early exit for iteration only.  At least there are only 8.
        if (yByteIndex < 0 || yByteIndex >= SCREEN_HEIGHT ||
            sprite[ySpriteIndex] == 0)
        {
            continue;
        }
        
        uint8_t leftBitOffset = xPixel % BITS_PER_BYTE;
        uint8_t xByteIndex = xPixel / BITS_PER_BYTE;
        uint8_t absoluteIndex = yByteIndex * (SCREEN_WIDTH / BITS_PER_BYTE) + xByteIndex;
        
        if (leftBitOffset != 0)
        {
            _memory[absoluteIndex] ^= sprite[ySpriteIndex] >> leftBitOffset;
            uint8_t rightBitOffset = BITS_PER_BYTE - leftBitOffset;
            overlaps = (_memory[absoluteIndex] & (sprite[ySpriteIndex] >> leftBitOffset)) != 0;
            
            if (xByteIndex < SCREEN_WIDTH)
            {
                _memory[absoluteIndex + 1] ^= sprite[ySpriteIndex] << rightBitOffset;
                overlaps = (_memory[absoluteIndex + 1] & (sprite[ySpriteIndex] << rightBitOffset)) != 0;
            }
            else
            {
                _memory[absoluteIndex - xPixel / BITS_PER_BYTE] ^= sprite[ySpriteIndex] << rightBitOffset;
                overlaps = (_memory[absoluteIndex - xPixel / BITS_PER_BYTE] & (sprite[ySpriteIndex] << rightBitOffset)) != 0;
            }
        }
        else
        {
            _memory[absoluteIndex] ^= sprite[ySpriteIndex];
            overlaps = (_memory[absoluteIndex] & sprite[ySpriteIndex]) != 0;
        }
    }
    
    return overlaps;
}

void Display::show()
{
    {
        // Drawing screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        SDL_Rect pixel = {
            0,
            0,
            REALPIXELS_PER_PIXEL,
            REALPIXELS_PER_PIXEL
        };
        
        for (int y = 0; y < SCREEN_HEIGHT; y++)
        {
            for (int x = 0; x < SCREEN_WIDTH; x++)
            {
                bool show = get(x, y);
                
                if (show)
                {
                    pixel.x = x * REALPIXELS_PER_PIXEL;
                    pixel.y = y * REALPIXELS_PER_PIXEL;
                    
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        
        SDL_RenderPresent(renderer);
    }
    
    {
        // Drawing debug
        SDL_SetRenderDrawColor(debugRenderer, 0, 0, 0, 255);
        SDL_RenderClear(debugRenderer);

        int strPosition = 10;
        int strSpacing = 20;
        std::stringstream ss;
        for (int registerIndex = 0; registerIndex < N_REGS; registerIndex++)
        {
            int registerValue = _registers[registerIndex];
            ss.str("");
            ss << std::hex << registerIndex << ": " << std::hex << registerValue;
            FontCache::DrawString(debugRenderer,
                    debugFont,
                    ss.str().c_str(),
                    10,
                    strPosition,
                    { 255, 255, 255 });
            strPosition += strSpacing;

        }
        ss.str("");
        ss << "I: " << std::hex << _I << "\n";
        FontCache::DrawString(debugRenderer,
                debugFont,
                ss.str().c_str(),
                10,
                strPosition,
                { 255, 255, 255 });
        strPosition += strSpacing;
        ss.str("");
        ss << "SP: " << std::hex << _SP << "\n";
        FontCache::DrawString(debugRenderer,
                debugFont,
                ss.str().c_str(),
                10,
                strPosition,
                { 255, 255, 255 });
        strPosition += strSpacing;
        ss.str("");
        ss << "PC: " << std::hex << _PC << "\n";
        FontCache::DrawString(debugRenderer,
                debugFont,
                ss.str().c_str(),
                10,
                strPosition,
                { 255, 255, 255 });
        strPosition += strSpacing;
        
        SDL_RenderPresent(debugRenderer);
    }
}

void Display::setRegisterValue(int index, uint8_t value)
{
    if (index < 0 || index >= N_REGS)
    {
        std::cout << "Display's register index out of bounds." << std::endl;
        // No point anymore :(
        exit(1);
    }

    _registers[index] = value;
}

void Display::setIRegisterValue(uint16_t value)
{
    _I = value;
}

void Display::setStackPointerValue(uint16_t value)
{
    _SP = value;
}

void Display::setProgramCounterValue(uint16_t value)
{
    _PC = value;
}
