#include <system16/system16.h>

/*
 *  This tests if/else clauses.  Each test in the if clause should result as false so the test passes
 *  if the 15 LS LEDs are on.
 */

int main()
{
    int *pLeds = LED_REG;
    *pLeds = 0;
    
    // false clauses
	if (0 + 0)
	{}
	else
	    *pLeds = *pLeds | 0x0001;
	if (1 - 1)
	{}
	else
	    *pLeds = *pLeds | 0x0002;
	if (1 & 0)
	{}
	else
	    *pLeds = *pLeds | 0x0004;
	if (0 | 0)
	{}
	else
	    *pLeds = *pLeds | 0x0008;
	if (1 && 0)
	{}
	else
	    *pLeds = *pLeds | 0x0010;
	if (0 || 0)
	{}
	else
	    *pLeds = *pLeds | 0x0020;
	if (!1)
	{}
	else
	    *pLeds = *pLeds | 0x0040;
	if (0 == 1)
	{}
	else
	    *pLeds = *pLeds | 0x0080;
	if (1 != 1)
	{}
	else
	    *pLeds = *pLeds | 0x0100;
	if (1 > 2)
	{}
	else
	    *pLeds = *pLeds | 0x0200;
	if (1 >= 2)
	{}
	else
	    *pLeds = *pLeds | 0x0400;
	if (1 >= 2)
	{}
	else
	    *pLeds = *pLeds | 0x0800;
	if (2 < 1)
	{}
	else
	    *pLeds = *pLeds | 0x1000;
	if (2 <= 1)
	{}
	else
	    *pLeds = *pLeds | 0x2000;
	if (2 <= 1)
	{}
	else
	    *pLeds = *pLeds | 0x4000;
}

