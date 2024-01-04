/*
 *  test.c
 *
 */

#include <system16/system16.h>
#include <system16/libasm.h>

void main()
{
    int curVal = 0, newVal;
    int isNewEntry = 1;
    int buttonCode;
    
    while (1)
    {
        //fputw(curVal, OP_DISPLAY);
        _Display(curVal);
        
        newVal = _AppendKeyValue(curVal, isNewEntry);
        if (newVal != curVal)
        {
            curVal = newVal;
            isNewEntry = 0;
        }
            
        buttonCode = _ReadButton();
        if (buttonCode == BUTTON_U)
        {
            isNewEntry = 1;
        }
        if (buttonCode == BUTTON_L)
        {
            isNewEntry = 1;
        }
        if (buttonCode == BUTTON_R)
        {
            isNewEntry = 1;
        }
/*        
        if (buttonCode == BUTTON_D)
        {
            isNewEntry = 1;
        }
*/        
    }
}

