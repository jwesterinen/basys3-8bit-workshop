/*
 *  Tests local and global array assignments.
 *
 *  The test passes if:
 *    - D1 = "1"
 *    - D2 = "2"
 *    - D3 = "3"
 *    - D4 = "4"
 *    - LEDs ="0000000000001111"
 */

#include <system16/system16.h>

int b[5];

int *pD1, *pD2, *pD3, *pD4;

void AssignArray(int b[], int n)
{
    b[n] = n;
}

void main()
{
    int a[5];
    int x, y;

    int *pLeds = LED_REG;
    *pLeds = 0;
    pD1 = DISPLAY1_REG;
    pD2 = DISPLAY2_REG;
    pD3 = DISPLAY3_REG;
    pD4 = DISPLAY4_REG;

    a[1] = 1;                       // local array assignment
    x = a[1];
    if (x == 1)
	    *pLeds = *pLeds | 0x0008;
    *pD1 = a[1];                    // should display 1            
	    
    b[2] = 2;
    x = b[2];                       // global array assignment    
    if (x == 2)
	    *pLeds = *pLeds | 0x0004;
    *pD2 = b[2];                    // should display 2            

    y = 3;
    AssignArray(a, y);
    x = a[y];                       // local array parameter
    if (x == y)
	    *pLeds = *pLeds | 0x0002;
    *pD3 = a[y];                    // should display 3            

    y = 4;
    AssignArray(b, y); 
    x = b[y];                       // global array parameter    
    if (x == y)
	    *pLeds = *pLeds | 0x0001;
    *pD4 = b[y];                    // should display 4
}


