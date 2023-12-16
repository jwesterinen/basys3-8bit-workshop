/*
 *  multiplication and division test
 *
 *  Test passes if:
 *      - LEDs: 0000 1111 1111 1111
 */

#include <system16/system16.h>
#include <system16/fputw.c>

int main()
{
    int results = 0;

    if (0 * 2 == 0)
	    results = results | 0x0001;	    
    if (4 * 0 == 0)
	    results = results | 0x0002;	    
    if (4 * 2 == 8)
	    results = results | 0x0004;	    
    if (4 * -2 == -8)
	    results = results | 0x0008;	    
    if (-4 * 2 == -8)
	    results = results | 0x0010;	    
    if (-4 * -2 == 8)
	    results = results | 0x0020;	    
    if (0 / 2 == 0)
	    results = results | 0x0040;	    
    if (8 / 0 == 0)
	    results = results | 0x0080;
    if (8 / 2 == 4)
	    results = results | 0x0100;
    if (8 / -2 == -4)
	    results = results | 0x0200;	    
    if (-8 / 2 == -4)
	    results = results | 0x0400;	    
    if (-8 / -2 == 4)
	    results = results | 0x0800;
	    
    fputw(results, OP_LEDS);
}

