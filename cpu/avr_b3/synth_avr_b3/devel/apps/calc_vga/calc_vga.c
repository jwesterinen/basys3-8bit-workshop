/*
*   calc.asm
*
*   This application implements an RPN calculator on the B3 VGA display.  The
*   keypad is used to enter decimal numbers (0-9) and functions.  The function
*   keys are:
*    - A: Enter
*    - B: Backspace
*    - C: Add
*    - D: Subtract
*
*   Note: the center button is the reset button
*
*   With an RPN calculator, numbers are entered then the functions are performed.
*   Pressing a function key displays the result of the function.
*
*   Test 1:
*       1. press 123
*       2. press Enter
*       3. press 456
*       4. press Enter
*       5. press Add
*       6. the value "579" should be displayed
*
*   Test 2:
*       1. press 2
*       2. press Enter
*       3. press 3
*       4. press Enter
*       5. press 4
*       6. press Enter
*       7. press 5
*       8. press Enter
*       9. press 6
*      10. press Enter
*      11. press Add
*      15. the value 11 should be displayed
*      12. press Add
*      15. the value 15 should be displayed
*      13. press Add
*      15. the value 18 should be displayed
*      14. press Add
*      15. the value 20 should be displayed
*/

#include <stdlib.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_lib.h"

// eval stack and its index, i.e. eval stack pointer
int es[10], esp = 0;

// push a onto stack
int Push(int a)
{
    stdout = &mystdout;
    printf("push %d onto eval stack...\r\n", a);
    
    es[esp++] = a;
    return a;
}

// pop the stack and return the value
int Pop(void)
{
    stdout = &mystdout;
    printf("pop %d from eval stack...\r\n", es[esp-1]);
    
    return (esp > 0) ? es[--esp] : es[0];
} 

// return TOS
int Top(void)
{
    return (esp > 0) ? es[esp-1] : es[0];
} 

// TOS = a
int Put(int a)
{
    stdout = &mystdout;
    printf("put %d onto top of eval stack...\r\n", a);
    
    if (esp > 0)
        es[esp-1] = a;
    return a;
}

int main(void)
{
    uint8_t keycode;
    char valueStr[20];
    int i = 0, value, b;
    bool entryComplete = false;

    // set UART baud rate to 115200
    UBRR0 = 13-1;

    stdout = &mystdout;
    printf("starting VGA calculator\r\n");

    // init the cursor to the bottom left
    VgaClearFrameBuffer();
    VGA_CUR_ROW = VGA_ROW_MAX;
    VGA_CUR_COL = 0;
    VGA_ROW_OFFSET = 0;
       
    VgaPrintStr("> ");
    while (1)
    {
        // read entry
        if ((keycode = ReadKeypad(false, 0, 0)) != KEY_NONE)
        {
            if (keycode < KEY_A)
            {
                valueStr[i++] = VgaPrintKeypadCode(keycode);
                entryComplete = false;
            }
            else
            {
                switch (keycode)
                {
                    // "Enter"
                    case KEY_A:
                        if (i > 0 && !entryComplete)
                        {
                            // convert entry string to int
                            valueStr[i] = (uint8_t)'\0';
                            value = atoi(valueStr);
                            
                            // push the current entry on the expr stack
                            Push(value);
                            
                            // setup for next entry
                            entryComplete = true;
                            i = 0;
                            VgaNewline();
                            VgaPrintStr("> ");
                        }
                        break;
                        
                    // "Backspace"                
                    case KEY_B:
                        // clear the last char entered
                        if (i > 0)
                        {
                            VGA_CUR_COL--;
                            VGA_CHAR = ' ';
                            VGA_CUR_COL--;
                            i--;
                        }
                        break;
                        
                    // "+"
                    case KEY_C:
                        if (entryComplete)
                        {
                            // acknowledge with plus sign
                            VgaPrintStr("+");
                            VgaNewline();
                            
                            // put the sum of the top 2 expr stack entries onto the top of the stack and set the new value
                            value = Put(Pop() + Top());
                            sprintf(valueStr, "%d", value);
                            VgaPrintStr(valueStr);
                            VgaNewline();
                            VgaPrintStr("> ");
                        }
                        break;
                        
                    // "-"
                    case KEY_D:
                        if (entryComplete)
                        {
                            // acknowledge with minus sign
                            VgaPrintStr("-");
                            VgaNewline();
                            
                            // put the difference of the top 2 expr stack entries onto the top of the stack and set the new value
                            b = Pop();
                            value = Put(Top() - b);
                            sprintf(valueStr, "%d", value);
                            VgaPrintStr(valueStr);
                            VgaNewline();
                            VgaPrintStr("> ");
                        }
                        break;
                }
            }
        }
    }
}

// end of calc_vga.c

