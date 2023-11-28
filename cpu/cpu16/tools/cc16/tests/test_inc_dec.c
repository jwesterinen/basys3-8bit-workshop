/*
 *  This tests pre/post inc/dec.  The test passes if the 6 LS LEDs are on.
 */

#include "system16.h"

int main()
{
    int *pLeds = LED_REG;
    int x = 5;

    // init the test results
    *pLeds = 0;

    // test pre-inc  
    ++x;                                // x = 6
	if (++x == 7)
	    *pLeds = *pLeds | 0x0001;
	    
	// test post-inc
    x++;                                // x = 8
	if (x++ == 8)
	    *pLeds = *pLeds | 0x0002;
	if (x == 9)
	    *pLeds = *pLeds | 0x0004;

    // test pre-dec
    --x;                                // x = 8
	if (--x == 7)
	    *pLeds = *pLeds | 0x0008;
	    
	// test post-dec
    x--;                                // x = 6
	if (x-- == 6)
	    *pLeds = *pLeds | 0x0010;
	if (x == 5)
	    *pLeds = *pLeds | 0x0020;
}

