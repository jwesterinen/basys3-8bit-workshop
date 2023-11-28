// test arrays

#include "system16.h"

int b[5];

int *pD1, *pD2, *pD3, *pD4;

void foo3(int n[])
{
    n[3] = 3;
}

void foo4(int n[])
{
    n[4] = 4;
}

void main()
{
    int a[5];
    int x;

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

    foo3(a);
    x = a[3];                       // local array parameter
    if (x == 3)
	    *pLeds = *pLeds | 0x0002;
    *pD3 = a[3];                    // should display 3            

    foo4(b); 
    x = b[4];                       // global array parameter    
    if (x == 4)
	    *pLeds = *pLeds | 0x0001;
    *pD4 = b[4];                    // should display 4
}


