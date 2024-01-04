/*
 *  calc.c
 *
 */

#include <system16/system16.h>
#include <system16/libasm.h>

int es[10], esp;

// push a onto stack
void Push(int a)
{
    es[esp] = a;
    esp = esp + 1;
}

// pop the stack and return the value
int Pop()
{
    esp = esp - 1;
    return es[esp + 1];
} 

// return TOS
int Top()
{
    return es[esp];
} 

// TOS = a
void Put(int a)
{
    es[esp] = a;
}

void main()
{
    int curVal = 0, newVal;
    int isNewEntry = 1;
    int buttonCode;
    
    while (1)
    {
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
            // push the current value on the expr stack
            Push(curVal);
            isNewEntry = 1;
        }
        if (buttonCode == BUTTON_L)
        {
            // put the sum of the top 2 expr stack entries onto the top of the stack
            Put(Pop() + Top());
            isNewEntry = 1;
        }
        if (buttonCode == BUTTON_R)
        {
            // put the difference of the top 2 expr stack entries onto the top of the stack
            //Put(Pop() - Top());
            isNewEntry = 1;
        }
        if (buttonCode == BUTTON_D)
        {
            // clear the current value
            curVal = 0;
            isNewEntry = 1;
        }
    }
}

