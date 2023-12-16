#include <hack_stdlib.h>

// CLR, +, -, *, /
#define KEYCODE_CLR         30
#define KEYCODE_PLUS        27
#define KEYCODE_MINUS       28
#define KEYCODE_MULTIPLY    29
#define KEYCODE_DIVIDE      31

// converts a digit keycode to an integer
#define KEYCODE_MASK        15

void main()
{
    int keycode;
    int *KBD = 24576;
    int *DISPLAY1 = 16384;
    
    ClearDisplay();
    
    while (1)
    {
        //keycode = *KBD;
        //if (keycode)
        if (*KBD)
        {
            //*DISPLAY1 = keycode & KEYCODE_MASK;
            *DISPLAY1 = *KBD & KEYCODE_MASK;
        }
    }
}

// end of test_keyout.c

