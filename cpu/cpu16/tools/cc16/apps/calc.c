/* calc -- A simple, 4-function RPN calculator
 *
 *  Test case 1 -- test all digits and functions including negative mult and div
 *   Enter      Display
 *   123         123
 *   <enter>     123
 *   456         456
 *   +           579
 *   789         789
 *   -          -210
 *   2           002
 *   *          -420
 *   10          010
 *   /          -042
 *   100         100
 *   +           058
 *   4           004
 *   *           232
 *   2           002
 *   /           116
 *
 *  Test case 2 -- non-trivial expression, (11+5)/(6+2) = 2
 *   <clr> 11 <enter> 5 + 6 <enter> 2 + /  should display 2
 *
 *  Test case 3 -- RPN city
 *   Enter      Display
 *   2           002
 *   <enter>     002
 *   3           003
 *   <enter>     003
 *   4           004
 *   <enter>     004
 *   5           005
 *   <enter>     005
 *   6           006
 *   <enter>     006
 *   +           011
 *   +           015
 *   +           018
 *   +           020
 * 
 */
 
#include <hack_stdlib.h>
#include <hackdefs.h>

// operation keys: CLR, +, -, *, /
#define KEYCODE_CLR         HACK_KEY_CNCL
#define KEYCODE_PLUS        HACK_KEY_UP
#define KEYCODE_MINUS       HACK_KEY_DOWN
#define KEYCODE_MULTIPLY    HACK_KEY_RIGHT
#define KEYCODE_DIVIDE      HACK_KEY_LEFT

// converts a digit keycode to an integer
#define KEYCODE_MASK        15

// expression stack
#define ExprPush(n) exprStack[++tos] = (n)
#define ExprPop()   exprStack[tos];     \
                    if (tos) --tos;
#define ExprTop()   exprStack[tos]
#define ExprPut(n)  exprStack[tos] = (n)
int exprStack[20];
int tos = 0;

void main()
{
    int keycode;
    int b;
    int isNewEntry = 1;
    
    ClearDisplay();
    
    while (1)
    {
        // get the next key pressed
        keycode = ReadKey();
        if (keycode)
        {
            if (keycode == KEYCODE_CLR)
            {
                // reset the system
                ResetSys();
                continue;
            }
            
            // accept and process entries
            if (keycode < HACK_KEY_ENTR)
            {
                // rotate a new digit into the display
                if (isNewEntry)
                {
                    ClearDisplay();
                }
                isNewEntry = 0;
                AppendNum3(keycode & KEYCODE_MASK);
            }
            else 
            {
                if (isNewEntry == 0)
                {
                    // push displayed value onto the expr stack for any non-digit key
                    ExprPush(Getsd());
                }
                if (keycode == KEYCODE_PLUS)
                {
                    // add: TOS = a + b
                    b = ExprPop();
                    ExprPut(ExprTop() + b);
                }
                else if (keycode == KEYCODE_MINUS)
                {
                    // sub: TOS = a - b
                    b = ExprPop();
                    ExprPut(ExprTop() - b);
                }
                else if (keycode == KEYCODE_MULTIPLY)
                {
                    // mul: TOS = a * b
                    b = ExprPop();
                    ExprPut(ExprTop() * b);
                }
                else if (keycode == KEYCODE_DIVIDE)
                {
                    // div: TOS = a / b
                    b = ExprPop();
                    ExprPut(ExprTop() / b);
                }
                
                // display the expression result
                Printsd(ExprTop());
                
                // flag a new entry so that the display will clear when a key is pressed
                isNewEntry = 1;
            }
        }
    }
}

// end of calc.c

