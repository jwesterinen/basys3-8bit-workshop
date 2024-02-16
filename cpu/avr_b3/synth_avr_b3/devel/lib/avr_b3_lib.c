/*
*   system_avr_lib
*
*   This file contains utilities to access avr_b3 peripherals.
*
*/

#define F_CPU 12500000UL
#include <stdbool.h>
#include <util/delay.h>
#include "../include/avr_b3.h"

void msleep(uint16_t msec)
{
    while (msec)
    {	
        _delay_loop_2((uint32_t)F_CPU/4000UL);
        msec--;
    }
}

void Display(uint16_t value, uint8_t displayQty)
{
    if (displayQty > 2)
    {
        DISPLAY0 = (value & 0xf000) >> 12;
        DISPLAY1 = (value & 0x0f00) >> 8;
    }
    DISPLAY2 = (value & 0x00f0) >> 4;
    DISPLAY3 = (value & 0x000f);
}

void KeyBeep(uint8_t tone, uint16_t durationMs)
{
    VCO1_FREQ_HI = 0;
    MIXER_SEL = MIXER_SEL_VCO1;
    VCO1_FREQ_LO = tone;
    msleep(durationMs);
    VCO1_FREQ_LO = 0;
}

uint8_t ReadKeypad(bool beep, uint8_t tone, uint16_t durationMs)
{
    uint8_t keyCode = KEYPAD;
    
    if (keyCode)
    {
        if (beep)
            //KeyBeep(0x80, 100);
            KeyBeep(tone, durationMs);
        while (KEYPAD)
        ;
    }
    
    return keyCode;
}

uint8_t ReadButtons(bool beep, uint8_t tone, uint16_t durationMs)
{
    uint8_t ButtonCode = BUTTONS;
    
    if (ButtonCode)
    {
        if (beep)
            //KeyBeep(0x80, 100);
            KeyBeep(tone, durationMs);
        while (BUTTONS)
        ;
    }
    
    return ButtonCode;
}

int AppendKeyValue(int value, bool *pIsNewEntry, bool beep, uint8_t tone, uint16_t durationMs)
{
    uint8_t keycode = ReadKeypad(beep, tone, durationMs);
    
    if (keycode)
    {
        if (*pIsNewEntry)
        {
            value = 0;
            *pIsNewEntry = false;
        }
        value = (value << 4) | (keycode & 0x000f);
    }
    
    return value;
}


