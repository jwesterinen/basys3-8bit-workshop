/*
*   calc.asm
*
*   This application implements an RPN calculator.  The keypad is used to enter
*   numbers shown on the display.  As keys are pressed, the corresponding number
*   is appended to the displayed value.  The buttons are used to perform 5 functions:
*    - Reset (middle button)
*    - Enter (top button)
*    - Clear entry (bottom button)
*    - Add (left button)
*    - Subtract (right button)
*
*   With an RPN calculator, numbers are entered then the functions are performed.
*   Pressing a function button displays the result of the function.
*
*   Test:
*       1. press "1234"
*       2. press Enter
*       3. press 2345
*       4. press Enter
*       5. press Add
*       6. the value "3579" should be displayed
*
*   Test case 3 -- RPN city
*       Enter      Display
*       2           0002
*       <enter>     0002
*       3           0003
*       <enter>     0003
*       4           0004
*       <enter>     0004
*       5           0005
*       <enter>     0005
*       6           0006
*       <enter>     0006
*       +           000b
*       +           000F
*       +           0012
*       +           0014
*/

#include "../../include/system_avr_b3.h"

// For linked compilation comment out the following AND add system_avr_b3.o to OBJS in the Makefile
#define MONOLITHIC

#ifdef MONOLITHIC

#include "../../include/system_avr_b3_lib.c"

#else

#include "../../include/system_avr_b3_lib.h"

#endif

// eval stack and its index, i.e. eval stack pointer
int es[10], esp;

// push a onto stack
int Push(int a)
{
    es[esp++] = a;
    return a;
}

// pop the stack and return the value
int Pop(void)
{
    return es[--esp];
} 

// return TOS
int Top(void)
{
    return es[esp-1];
} 

// TOS = a
int Put(int a)
{
    es[esp-1] = a;
    return a;
}

int main(void)
{
    int value = 0, b;
    bool isNewEntry = true;
   
    while (1)
    {
        Display(value, 4);
        
        value = AppendKeyValue(value, &isNewEntry, true);
            
        switch (ReadButtons(true))
        {
            // "Enter"
            case BUTTON_U:
                // push the current value on the expr stack
                Push(value);
                isNewEntry = true;
                break;
                
            // "+"
            case BUTTON_L:
                // put the sum of the top 2 expr stack entries onto the top of the stack and set the new value
                value = Put(Pop() + Top());
                isNewEntry = true;
                break;
                
            // "-"
            case BUTTON_R:
                // put the difference of the top 2 expr stack entries onto the top of the stack and set the new value
                b = Pop();
                value = Put(Top() - b);
                isNewEntry = true;
                break;
                
            // "Clear Entry"                
            case BUTTON_D:
                // clear the current value
                value = 0;
                isNewEntry = true;
                break;
        }
    }
}

