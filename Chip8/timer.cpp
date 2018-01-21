//
//  timer.cpp
//  Chip8
//
//  Created by Koissi Adjorlolo on 3/12/16.
//  Copyright © 2016 centuryapps. All rights reserved.
//

#include "timer.hpp"

uint8_t Timer::get()
{
    return _value;
}

void Timer::set(uint8_t value)
{
    _value = value;
}

void Timer::update()
{
    if (!isZero())
    {
        _value--;
    }
}

void Timer::reset()
{
    _value = 0;
}

bool Timer::isZero()
{
    return _value == 0;
}
