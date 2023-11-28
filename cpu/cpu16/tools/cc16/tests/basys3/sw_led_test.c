// write switch reg value to the LEDs

#include <hack_stdlib.h>

void main()
{
    int* switchReg = 28675;
    int* ledReg = 28672;
    
    // read the switch register and write the value to the LEDs
    while (1)
    {
        *ledReg = *switchReg;
    }
}
