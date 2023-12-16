/*  ALU test 
 *
 *  Test arithmetic, bitwise, and assignment operators
 *
 *  Test passes if 
 *    - display = 003F
 *    - LEDs = 0111 1111 1111 1111
 */

#include <system16/libasm.h>

int main()
{
    int x = 0, y = 0, z;
    
    // preliminary tests
    if (1 == 1)
        x |= 0x0001;
    if (1 == 2)
        x |= 0x0000;
    else
        x |= 0x0002;
    z = 0;
    z |= 0xff;
    if (z == 0xff)
        x |= 0x0004;

    // arithmetic operators
	if (2 + 1 == 3)
	    x |= 0x0008;
	if (2 - 1 == 1)
	    x |= 0x0010;
	if (2 * 3 == 6)
	    x |= 0x0020;
	if (10 / 5 == 2)
	    x |= 0x0040;
    /*    
	if (2 % 1 == 3)
	    x |= 0x0000;
	*/
	
	z = 5;
	++z;
	if (z == 6)
	    x |= 0x0080;	    
	--z;
	if (z == 5)
	    x |= 0x0100;
	if (z++ == 5)
	    x |= 0x0200;	    
	if (z-- == 6)
	    x |= 0x0400;
	    
    // bitwise operators	    
	// TODO: is this correct??
	if ((2 & 1) == 0)
	    x |= 0x0800;	
	if (2 | 1 == 3)
	    x |= 0x1000;	    
	if (2 ^ 1 == 3)
	    x |= 0x2000;
	if (~0xa5a5 == 0x5a5a)
	    x |= 0x4000;	    
    /*        
	if (2 << 3 == 8)
	    x |= 0x0000;
	if (16 >> 3 == 4)
	    x |= 0x0000;
	*/


	    
    // assignment operators
    z = 5;
	z += 1;
	if (z == 6)
	    y |= 0x0001;
	z -= 1;
	if (z == 5)
	    y |= 0x0002;
	z *= 2;
	if (z == 10)
	    y |= 0x0004;
	z /= 5;
	if (z == 2)
	    y |= 0x0008;
	/*
	z %= 5;
	if (z == 2)
	    y |= 0x0000;
	z <<= 5;
	if (z == 2)
	    y |= 0x0000;
	z >>= 5;
	if (z == 2)
	    y |= 0x0000;
	*/		    
	z &= 7;
	if (z == 2)
	    y |= 0x0010;
	z |= 5;
	if (z == 7)
	    y |= 0x0020;
	/*
	z ^= 16;
	if (z == 10)
	    y |= 0x0000;
	*/
	    
	_ShowLeds(x);
	_Display(y);
}

