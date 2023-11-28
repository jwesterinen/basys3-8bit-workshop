/*
 *  fputw
 *
 *  Synopsis: void fputw(int value, int device);
 *
 *  Parameters:
 *      value:  the value to be displayed
 *      device: the device on which to display the value as follows:
 *          OD_LEDS:      system16 LEDs
 *          OD_DISPLAY:   system16 4-segment display
 *  
 */

// output devices
#define OD_LEDS     0x00
#define OD_DISPLAY  0x01 

int fputw(int value, int device)
{
    int retval = 0;

/*    
    if (device == OD_LEDS)
    {
        int *pLeds = LED_REG;
        *pLeds = value;
    }
    else if (device == OD_DISPLAY)
    {
        _Display(value);
    }
    else
    {
        retval = -1;
    }
*/    
    _Display(value);
    
    return retval;
    
} // end of fputw


