//
//  keyboard.cpp
//  Chip8
//
//  Created by Koissi Adjorlolo on 3/11/16.
//  Copyright © 2016 centuryapps. All rights reserved.
//

#include <SDL2/SDL.h>
#include <iostream>
#include <map>
#include "keyboard.hpp"

static SDL_Event event;

Keyboard::Keyboard()
{
    if (SDL_Init(SDL_INIT_EVENTS))
    {
        std::cout << "Unable to init keyboard." << std::endl;
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
}

static std::map<SDL_Keycode, uint8_t> CONVERSION_MAP = {
    { SDLK_1, 0x1 },
    { SDLK_2, 0x2 },
    { SDLK_3, 0x3 },
    { SDLK_4, 0xC },
    { SDLK_q, 0x4 },
    { SDLK_w, 0x5 },
    { SDLK_e, 0x6 },
    { SDLK_r, 0xD },
    { SDLK_a, 0x7 },
    { SDLK_s, 0x8 },
    { SDLK_d, 0x9 },
    { SDLK_f, 0xE },
    { SDLK_z, 0xA },
    { SDLK_x, 0x0 },
    { SDLK_c, 0xB },
    { SDLK_v, 0xF },
};

void Keyboard::clear()
{
    for (int keyIndex = 0; keyIndex < N_KEYS; keyIndex++)
    {
        _keys[keyIndex] = 0;
    }
}

bool Keyboard::poll()
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                return false;
                
            case SDL_KEYDOWN:
                if (CONVERSION_MAP.find(event.key.keysym.sym) !=
                    CONVERSION_MAP.end())
                {
                    _keys[CONVERSION_MAP[event.key.keysym.sym]] = true;
                }
                break;
                
            case SDL_KEYUP:
                if (CONVERSION_MAP.find(event.key.keysym.sym) !=
                    CONVERSION_MAP.end())
                {
                    _keys[CONVERSION_MAP[event.key.keysym.sym]] = false;
                }
                break;
                
            default:
                break;
        }
    }
    
    return true;
}

bool Keyboard::isKeyPressed(uint8_t key)
{
    return key >= 0 && key < N_KEYS && _keys[key];
}

uint8_t Keyboard::waitForKeyPress()
{
    bool shouldReturn;
    uint8_t returnKey = 0;
    
    while (!shouldReturn)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    return UINT8_MAX;
                    
                case SDL_KEYDOWN:
                    returnKey = CONVERSION_MAP[event.key.keysym.sym];
                    shouldReturn = true;
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    return returnKey;
}
