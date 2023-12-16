/*  ALU test 
 *
 *  Test logical and relational operators
 *
 *  Test passes if 
 *    - display = 0FFF
 *    - LEDs = 0000 0011 1111 1111
 */

#include <system16/libasm.h>

int main()
{
    int x = 0, y = 0, z;
    
    // logical operators	
	if (2 && 1)
	    x |= 0x0001;
	if (2 && 0)
	    x |= 0x0000;
	else
	    x |= 0x0002;
	if (0 && 1)
	    x |= 0x0000;
	else
	    x |= 0x0004;
	if (0 && 0)
	    x |= 0x0000;
	else
	    x |= 0x0008;

	if (2 || 1)
	    x |= 0x0010;
	if (2 || 0)
	    x |= 0x0020;
	if (0 || 2)
	    x |= 0x0040;
	if (0 || 0)
	    x |= 0x0000;
	else
	    x |= 0x0080;

	if (!0)
	    x |= 0x0100;
	if (!1)
	    x |= 0x0000;
	else
	    x |= 0x0200;

    // relational operators
	if (1 != 2)
	    y |= 0x0001;
	if (1 != 1)
	    y |= 0x0000;
	else
	    y |= 0x0002;

	if (2 > 1)
	    y |= 0x0004;
	if (1 > 2)
	    y |= 0x0000;
	else
	    y |= 0x0008;

	if (1 < 2)
	    y |= 0x0010;
	if (2 < 1)
	    y |= 0x0000;
	else
	    y |= 0x0020;

	if (2 >= 1)
	    y |= 0x0040;
	if (2 >= 2)
	    y |= 0x0080;
	if (1 >= 2)
	    y |= 0x0000;
	else
	    y |= 0x0100;

	if (1 <= 2)
	    y |= 0x0200;
	if (1 <= 1)
	    y |= 0x0400;
	if (2 <= 1)
	    y |= 0x0000;
	else
	    y |= 0x0800;
	    
	_ShowLeds(x);
	_Display(y);
}

