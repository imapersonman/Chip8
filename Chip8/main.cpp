//
//  main.cpp
//  Chip8
//
//  Created by Koissi Adjorlolo on 3/11/16.
//  Copyright © 2016 centuryapps. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include "chip8.hpp"

static const double FRAMES_PER_SECOND = 60.0;
static const double MS_PER_UPDATE = 1000 / FRAMES_PER_SECOND;

int main(int argc, const char * argv[])
{
    const char *filePath = "ROMS/PONG2";
    
    if (argc >= 2)
    {
        filePath = argv[1];
    }
    
    Chip8 *chip8 = new Chip8();
    
    double previous = SDL_GetTicks();
    double lag = 0.0;
    
    if (!chip8->loadROM(filePath))
    {
        std::cout << "Unable to load ROM" << std::endl;
        delete chip8;
        exit(1);
    }
    
    while (chip8->isRunning())
    {
        double current = SDL_GetTicks();
        double elapsed = current - previous;
        lag += elapsed;
        previous = current;
        
        while (lag >= MS_PER_UPDATE)
        {
            if (!chip8->isRunning())
            {
                break;
            }
            
            chip8->execute();
            lag -= MS_PER_UPDATE;
        }
    }
    
    return 0;
}
