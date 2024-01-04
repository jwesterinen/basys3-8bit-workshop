/*  ALU test 
 *
 *  Test arithmetic, bitwise, logical and relational operators on local direct variables.
 *
 *  Test passes if 
 *    - display = FFFF
 *    - LEDs = 1111 1111 1111 1111
 */

#include <system16/libasm.h>

int main()
{
    int x = 0, y = 0;

    // arithmetic operators
	if (2 + 1 == 3)
	    x = x | 0x0001;
	if (2 - 1 == 1)
	    x = x | 0x0002;
	if (2 * 3 == 6)
	    x = x | 0x0004;
	if (10 / 5 == 2)
	    x = x | 0x0008;
	if (300 % 41 == 13)
	    x = x | 0x0010;
	    
    // bitwise operators	    
	if ((2 & 1) == 0)   // NOTE: this is an example of the wrong precedence in C, == has higher precedence than & but shouldn't
	    x = x | 0x0020;	
	if (2 | 1 == 3)
	    x = x | 0x0040;	    
	if (2 ^ 1 == 3)
	    x = x | 0x0080;
	if (~0xa5a5 == 0x5a5a)
	    x = x | 0x0000;
	// FIXME: bad op precedence	-- shift is higher than equals
	if ((2 << 3) == 16)
	    x = x | 0x0100;
	if ((16 >> 3) == 2)
	    x = x | 0x0200;

    // logical operators	
	if (2 && 1)
	    x = x | 0x0400;
	if (2 && 0)
	    x = x | 0x0000;
	else
	    x = x | 0x0800;
	if (0 && 1)
	    x = x | 0x0000;
	else
	    x = x | 0x1000;
	if (0 && 0)
	    x = x | 0x0000;
	else
	    x = x | 0x2000;

	if (2 || 1)
	    x = x | 0x4000;
	if (2 || 0)
	    x = x | 0x8000;
	if (0 || 2)
	    y = y | 0x0001;
	if (0 || 0)
	    y = y | 0x0000;
	else
	    y = y | 0x0002;

	if (!0)
	    y = y | 0x0004;
	if (!1)
	    y = y | 0x0000;
	else
	    y = y | 0x0008;

    // relational operators
	if (1 != 2)
	    y = y | 0x0010;
	if (1 != 1)
	    y = y | 0x0000;
	else
	    y = y | 0x0020;

	if (2 > 1)
	    y = y | 0x0040;
	if (1 > 2)
	    y = y | 0x0000;
	else
	    y = y | 0x0080;

	if (1 < 2)
	    y = y | 0x0100;
	if (2 < 1)
	    y = y | 0x0000;
	else
	    y = y | 0x0200;

	if (2 >= 1)
	    y = y | 0x0400;
	if (2 >= 2)
	    y = y | 0x0800;
	if (1 >= 2)
	    y = y | 0x0000;
	else
	    y = y | 0x1000;

	if (1 <= 2)
	    y = y | 0x2000;
	if (1 <= 1)
	    y = y | 0x4000;
	if (2 <= 1)
	    y = y | 0x0000;
	else
	    y = y | 0x8000;
	    
	_ShowLeds(x);
	_Display(y);
}

