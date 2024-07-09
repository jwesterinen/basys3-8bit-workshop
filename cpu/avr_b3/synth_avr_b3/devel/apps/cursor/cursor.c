/*
*   cursor_test.c
*
*   This is a canonical cursor test of the VGA system.
*
*   The cursor starts in the middle of the VGA display. The buttons move the
*   cursor, BUTTON_U up, BUTTON_D down, etc.
*
*/

#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"

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
                VgaMoveCursor(CUR_UP);
                break;
                
            // cursor down
            case BUTTON_D:
                VgaMoveCursor(CUR_DOWN);
                break;
                
            // cursor left
            case BUTTON_L:
                VgaMoveCursor(CUR_LEFT);
                break;
                
            // cursor right
            case BUTTON_R:
                VgaMoveCursor(CUR_RIGHT);
                break;
        }
    
        // display keypad value
        if ((keypadCode = ReadKeypad(false, 0, 0)) != 0)
        {
            VgaPrintKeypadCode(keypadCode);
        }
    }
}

int main(void)
{
    CursorTest(VGA_ROW_MAX/2, VGA_COL_MAX/2);
}

