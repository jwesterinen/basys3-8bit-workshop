/*
 *  fputw.c
 *
 *  Description: Write a word to an output peripheral:
 *  
 *  Synopsis: void fputw(int value, int peripheral);
 *
 *  Parameters:
 *      value:      the value to be displayed
 *      peripheral: the peripheral on which to display the value as follows:
 *          OD_LEDS:      system16 LEDs
 *          OD_DISPLAY:   system16 4-segment display
 *  
 */

#include <system16/libasm.h>

// output peripherals
#define OP_LEDS     0x00
#define OP_DISPLAY  0x01 

void fputw(int value, int peripheral)
{
    if (peripheral == OP_LEDS)
    {
        int *pLeds = LED_REG;
        *pLeds = value;
    }
    else if (peripheral == OP_DISPLAY)
    {
        _Display(value);
    }
        
} // end of fputw

