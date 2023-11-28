/*
 *  fputw
 *
 *  Synopsis: void fputw(int value, OUTPUT_DEVICE device);
 *
 *  Description: Write a word to one of the following output devices:
 *    OD_LEDS:      system16 LEDs
 *    OD_DISPLAY:   system16 4-segment display
 *  
 */

// output devices
#define OD_LEDS     0x00
#define OD_DISPLAY  0x01 

void fputw(int value, OUTPUT_DEVICE device)
{

    if (device == OD_LEDS)
    {
        int *pLeds = LED_REG;
        *pLeds = value;
    }
    else
    {
        _Display(value);
    }
    
} // end of fputw


