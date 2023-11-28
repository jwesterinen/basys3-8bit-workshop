#include "system16.h"

int *gptr;          // global pointer
int a;

void main()
{
    int *pDisplay1 = DISPLAY1_REG;
    int *pDisplay2 = DISPLAY2_REG;
    int *pDisplay3 = DISPLAY3_REG;
    int *pDisplay4 = DISPLAY4_REG;
    
    int x, y, *px;
    
    px = &x;        // assignment to reference
    *px = 7;        // indirect assignment
    y = *px;        // assignment to indirect
    *pDisplay1 = y; // display '7' in D1

    foo(&x);        // pass a reference
    *pDisplay2 = x; // display '3' in D2      
    
    gptr = &y;      // assignment to global reference
    *gptr = 8;      // indirect assignment via global pointer
    a = *gptr;      // global var assignment via global indirect
    *pDisplay3 = a; // display '8' in D3      
}

void foo(int *px)
{
    *px = 3;
}

