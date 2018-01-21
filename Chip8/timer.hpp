//
//  timer.hpp
//  Chip8
//
//  Created by Koissi Adjorlolo on 3/12/16.
//  Copyright © 2016 centuryapps. All rights reserved.
//

#ifndef timer_hpp
#define timer_hpp

#include <stdio.h>
#include <stdint.h>

class Timer
{
public:
    Timer() : _value(0) {}
    uint8_t get();
    void set(uint8_t value);
    void update();
    void reset();
    bool isZero();
    
private:
    uint8_t _value;
};

#endif /* timer_hpp */
