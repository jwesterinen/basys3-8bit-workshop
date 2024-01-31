#include "../include/system_avr_b3.h"

int main(void)
{
    while (1)
    {
        // write switches to LEDs
        LED_LSB = SW_LSB;
        LED_MSB = SW_MSB;
        
        // write keypad to display 0
        DISPLAY0 = KEYPAD & 0x0f;
        
        // write buttons to display 1-3
        DISPLAY1 = BUTTONS;
        DISPLAY2 = BUTTONS;
        DISPLAY3 = BUTTONS;
    }
    
    return(0);
}

