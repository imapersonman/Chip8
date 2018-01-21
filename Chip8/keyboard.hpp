//
//  keyboard.hpp
//  Chip8
//
//  Created by Koissi Adjorlolo on 3/11/16.
//  Copyright © 2016 centuryapps. All rights reserved.
//

#ifndef keyboard_hpp
#define keyboard_hpp

#include <stdio.h>

static const uint8_t N_KEYS = 0x010;

class Keyboard
{
public:
    Keyboard();
    // Sets all keys to false.
    void clear();
    // To be called once per update
    // Returns false when it is time to quit.
    bool poll();
    bool isKeyPressed(uint8_t key);
    uint8_t waitForKeyPress();
private:
    bool _keys[N_KEYS];
};

#endif /* keyboard_hpp */
