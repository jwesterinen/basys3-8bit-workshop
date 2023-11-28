#include "system16.h"
//#include "../libc/fputw.h"

//int *pLeds = LED_REG;

void _Display(int value);

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

int main()
{
    int x;
    int y;
    int *z;

    z = &y;
    y = 0x2345;
    //x = y;
    //z = &y;
    x = *z;
    //x = foo();

    //_Display(x);    
    //*pLeds = bar;
    fputw(x, OD_DISPLAY);
}

/*
int foo()
{
    return 7;
}

int bar()
{
}
*/


