/*
*   vga.c
*/

#include "../../include/avr_b3.h"
//#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_lib.h"

void CursorTest(int row, int col)
{
    uint8_t keypadCode;
    
    VGA_CUR_ROW = row;
    VGA_CUR_COL = col;
    
    while (1)
    {
        // move cursor
        switch (ReadButtons(true, 0x02, 100))
        {
            // cursor up
            case BUTTON_U:
                MoveCursor(CUR_UP);
                break;
                
            // cursor down
            case BUTTON_D:
                MoveCursor(CUR_DOWN);
                break;
                
            // cursor left
            case BUTTON_L:
                MoveCursor(CUR_LEFT);
                break;
                
            // cursor right
            case BUTTON_R:
                MoveCursor(CUR_RIGHT);
                break;
        }
    
        // display keypad value
        if ((keypadCode = ReadKeypad(false, 0, 0)) != 0)
        {
            PrintKeypadCode(keypadCode);
        }
    }
}

int main(void)
{
    CursorTest(VGA_ROW_MAX/2, VGA_COL_MAX/2);
}

