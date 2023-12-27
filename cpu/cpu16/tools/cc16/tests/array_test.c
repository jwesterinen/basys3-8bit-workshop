/*
 *  Tests local and global array assignments.
 *
 *  The test passes if:
 *    - D1 = "1"
 *    - D2 = "2"
 *    - D3 = "3"
 *    - D4 = "4"
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

    pD1 = DISPLAY1_REG;
    pD2 = DISPLAY2_REG;
    pD3 = DISPLAY3_REG;
    pD4 = DISPLAY4_REG;

    a[1] = 1;               // local array assignment
    *pD1 = a[1];            // should display 1            
	    
    b[2] = 2;
    *pD2 = b[2];            // should display 2            

    AssignArray(a, 3);
    *pD3 = a[3];            // should display 3            

    AssignArray(b, 4); 
    *pD4 = b[4];            // should display 4
}


