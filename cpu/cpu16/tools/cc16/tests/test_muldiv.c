// multiplication and division test

#include <system16/system16.h>
#include <system16/fputw.c>
#include <system16/fputb.c>

int main()
{
    int x;
    int results = 0;

    x = 0 * 2;
    if (x == 0)
	    results = results | 0x0001;
	    
    x = 4 * 0;
    if (x == 0)
	    results = results | 0x0002;
	    
    x = 4 * 2;
    if (x == 8)
	    results = results | 0x0004;
	    
    x = 4 * -2;
    if (x == -8)
	    results = results | 0x0008;
	    
    x = -4 * 2;
    if (x == -8)
	    results = results | 0x0010;
	    
    x = -4 * -2;
    if (x == 8)
	    results = results | 0x0020;
	    
    x = 0 / 2;
    if (x == 0)
	    results = results | 0x0040;
	    
    x = 8 / 0;
    if (x == 0)
	    results = results | 0x0080;

    x = 8 / 2;
    if (x == 4)
	    results = results | 0x0100;

    x = 8 / -2;
    if (x == -4)
	    results = results | 0x0200;
	    
    x = -8 / 2;
    if (x == -4)
	    results = results | 0x0400;
	    
    x = -8 / -2;
    if (x == 4)
	    results = results | 0x0800;
	    
    fputw(results, OP_LEDS);
    fputb(1, OP_DISPLAY1);	    
    fputb(2, OP_DISPLAY2);	    
    fputb(3, OP_DISPLAY3);	    
    fputb(4, OP_DISPLAY4);	    
}

