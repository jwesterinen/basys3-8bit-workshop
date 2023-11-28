// classic blinking LEDs

#include <hack_stdlib.h>

void main()
{
    int *led = 28672;
    
    *led = 1;
    DelayMs(500);
    
    *led = 0;
    DelayMs(500);
}
