/*
 *  fputb.c
 *
 *  Description: Write a byte to an output peripheral:
 *  
 *  Synopsis: void fputb(int value, int peripheral);
 *
 *  Parameters:
 *      value:      the value to be displayed
 *      peripheral: the peripheral on which to display the value as follows:
 *          OD_DISPLAY1:   system16 display1
 *          OD_DISPLAY2:   system16 display2
 *          OD_DISPLAY3:   system16 display3
 *          OD_DISPLAY4:   system16 display4
 *  
 */

#include <system16/libasm.h>

// output peripherals
#define OP_DISPLAY1 0x02 
#define OP_DISPLAY2 0x03 
#define OP_DISPLAY3 0x04 
#define OP_DISPLAY4 0x05 

void fputb(int value, int peripheral)
{
    int *pDisplay;
    
    if (peripheral == OP_DISPLAY1)
    {
        pDisplay = DISPLAY1_REG;
        *pDisplay = value;
    }
    else if (peripheral == OP_DISPLAY2)
    {
        pDisplay = DISPLAY2_REG;
        *pDisplay = value;
    }
    else if (peripheral == OP_DISPLAY3)
    {
        pDisplay = DISPLAY3_REG;
        *pDisplay = value;
    }
    else if (peripheral == OP_DISPLAY4)
    {
        pDisplay = DISPLAY4_REG;
        *pDisplay = value;
    }
    
} // end of fputb

