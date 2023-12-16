/*
 *  Test for local and global loading and storing of const, direct, ref, deref, fct.
 *
 *  The test passes if:
 *    - D1 = "1"
 *    - D2 = "2"
 *    - D3 = "2"
 *    - D4 = "3"
 *    - LEDs ="0000111111111101"
 */

#include <system16/system16.h>

int *pLeds;
int *pDisplay1;
int *pDisplay4;

int glbVar;
int *glbPtr;

int foo()
{
    return 0x3;
}

int main()
{
    int a, b;
    int c = 0x2;
    int *d;
    int e, f;
    int *pDisplay2 = DISPLAY2_REG;  // local load/store
    int *pDisplay3;

    // local pointer assignments
    pLeds = LED_REG;                // global load, local store
    pDisplay1 = DISPLAY1_REG;       // local load, global store
    pDisplay3 = DISPLAY3_REG;       // local load, local store
    pDisplay4 = DISPLAY4_REG;       // global load, local store

    a = 0x1;                        // const load, local store
    glbPtr = &c;                    // local ref load, global store
    b = *glbPtr;                    // global pointer load, local store
    d = &b;                         // local ref load, local store
    e = *d;                         // local pointer load, local store
    glbVar = foo();                 // fct load, local store
    f = glbVar;

    // display a in D1, b in D2, e in D3, f in D4, d in display
    *pDisplay1 = b;                 // setup for consecutive global memory assignments of the same address
    *pDisplay1 = a;                 // global pointer store
    *pDisplay2 = a;                 // setup for consecutive local memory assignments of the same address
    *pDisplay2 = b;                 // local pointer store
    *pDisplay3 = e;
    *pDisplay4 = f;
    *pLeds = d;
}

