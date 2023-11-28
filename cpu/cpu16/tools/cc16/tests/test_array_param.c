// test arrays

#include "system16.h"

int b[5];
int n;

void main()
{
    int a[5];
    int x;
    int *pD1 = DISPLAY1_REG;
    int *pD2 = DISPLAY2_REG;

    x = 2;
    //foo(a, x, 5);   // local array parameter
    a[x] = 5;
    *pD1 = a[x];    // should display 5            
    
    n = 3;
    foo(b, n, 6);   // global array parameter    
    *pD2 = b[n];    // should display 6
}

void foo(int n[], int s, int val)
{
    n[s] = val;
}


