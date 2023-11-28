// classic blinking LEDs

#include <hack_stdlib.h>
//#include <hackdefs.h>

void main()
{
    //int keycode;
    int delay = 500;
    
    //SetGpioDir(1);
    while (1)
    {
        /*
        keycode = ReadKey();
        if (keycode)
        {
            if (keycode == HACK_KEY_UP)
            {
                // speed up blinking
                delay += 25;
            }
            if (keycode == HACK_KEY_DOWN)
            {
                // slow down blinking
                delay -= 25;
            }
        }
        */
        
        // blink the LED and GPIO
        //SetGpio(1);
        SetLed(0);
        DelayMs(500);
        //DelayMs(delay);
        //SetGpio(0);
        SetLed(1);
        //DelayMs(delay);
        DelayMs(500);
    }
}
