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
#include "../include/avr_b3_lib.h"

#define TABSIZE 4

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
    MIXER = VCO1;
    VCO1_FREQ = tone;
    msleep(durationMs);
    VCO1_FREQ = 0;
}

uint8_t ReadKeypad(bool beep, uint8_t tone, uint16_t durationMs)
{
    uint8_t keyCode = KEYPAD;
    
    if (keyCode)
    {
        if (beep)
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
            KeyBeep(tone, durationMs);
        while (BUTTONS)
        ;
    }
    
    return ButtonCode;
}

int AppendKeypadValue(int value, bool *pIsNewEntry, bool beep, uint8_t tone, uint16_t durationMs)
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

uint8_t VgaPrintKeypadCode(uint8_t keypadCode)
{
    uint8_t asciiVal;
    
    // decode the keypad code, convert it to ASCII, then print it
    keypadCode &= 0x0f;
    asciiVal = keypadCode + ((keypadCode <= 9) ? 0x30 : 0x37);
    VGA_CHAR = asciiVal;
    
    return asciiVal;
}

void VgaMoveCursor(enum VGA_CUR_DIR dir)
{
    switch (dir)
    {
        // cursor up
        case CUR_UP:
            // move the cursor up
            VGA_CUR_ROW -= (VGA_CUR_ROW > VGA_ROW_MIN) ? 1 : 0;
            break;
            
        // cursor down
        case CUR_DOWN:
            // move the cursor down
            VGA_CUR_ROW += (VGA_CUR_ROW < VGA_ROW_MAX) ? 1 : 0;
            break;
            
        // cursor left
        case CUR_LEFT:
            // move the cursor left
            VGA_CUR_COL -= (VGA_CUR_COL > VGA_COL_MIN) ? 1 : 0;
            break;
            
        // cursor right
        case CUR_RIGHT:
            // move the cursor right
            VGA_CUR_COL += (VGA_CUR_COL < VGA_COL_MAX) ? 1 : 0;
            break;
    }
}

char VgaGetChar(int row, int col)
{
    if ((VGA_ROW_MIN <= row && row <= VGA_ROW_MAX) && (VGA_COL_MIN <= col && col <= VGA_COL_MAX))
    {
        VGA_CUR_ROW = row;
        VGA_CUR_COL = col;
        return VGA_CHAR;    
    }
    return 0;
}

char VgaPutChar(int row, int col, char c)
{
    if ((VGA_ROW_MIN <= row && row <= VGA_ROW_MAX) && (VGA_COL_MIN <= col && col <= VGA_COL_MAX))
    {
        VGA_CUR_ROW = row;
        VGA_CUR_COL = col;
        VGA_CHAR = c;   
        return c;    
    } 
    return 0;
}

void VgaFillFrameBuffer(char c)
{
    VGA_CUR_ROW = VGA_ROW_MIN;
    VGA_CUR_COL = VGA_COL_MIN;
    for (int i = 0; i < (VGA_ROW_MAX+1)*(VGA_COL_MAX+1); i++)
        VGA_CHAR = c;
}

void VgaLoadFrameBuffer(VGA_DISPLAY_BUFFER srcBuf)
{
    for (int row = 0; row <= VGA_ROW_MAX; row++)
        for (int col = 0; col <= VGA_COL_MAX; col++)
            VgaPutChar(VGA_ROW_MIN + row, VGA_COL_MIN + col, srcBuf[row][col]);
}

void VgaFillDisplayBuffer(VGA_DISPLAY_BUFFER buffer, char c)
{
    for (int row = 0; row <= VGA_ROW_MAX; row++)
        for (int col = 0; col <= VGA_COL_MAX; col++)
            buffer[row][col] = c;
}

void VgaLoadDisplayBuffer(VGA_DISPLAY_BUFFER destBuf, VGA_DISPLAY_BUFFER srcBuf)
{
    for (int row = 0; row <= VGA_ROW_MAX; row++)
        for (int col = 0; col <= VGA_COL_MAX; col++)
            destBuf[row][col] = srcBuf[row][col];
}

void VgaNewline(void)
{
    // scroll the display up one row
    VGA_ROW_OFFSET = (VGA_ROW_OFFSET == 39) ? 0 : VGA_ROW_OFFSET+1;
    
    // reposition the cursor at the last line on the screen
    VGA_CUR_ROW = VGA_ROW_MAX;
    
    // clear the line
    for (int i = VGA_COL_MIN; i < VGA_COL_MAX; i++)
    {
        VGA_CUR_COL = i;
        VGA_CHAR = 0x20;
    }
    VGA_CUR_COL = VGA_COL_MIN;
}

void VgaPrintStr(char *str)
{
    int i = 0;
    
    while (str[i])
    {
        if (str[i] == '\t')
        {
            for (int j = 0; j < TABSIZE; j++)
                VGA_CHAR = ' ';
            i++;
        }
        else
            VGA_CHAR = str[i++];
    }
}


