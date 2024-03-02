/*
*/

#include "../../include/avr_b3.h"

//#define RAM_SIZE 0x0fff
//#define RAM_SIZE 0x0060
#define RAM_SIZE 10


int main(void)
{
    uint8_t *addr;
    uint16_t n;

    addr = (uint8_t *)0x060;
    //addr = (uint8_t *)0x100;
    for (n = 0; n < RAM_SIZE; n++)
    {
        *addr++ = n % 0x100;
    }

    addr = (uint8_t *)0x060;
    //addr = (uint8_t *)0x100;
    for (n = 0; n < RAM_SIZE; n++)
    {
        LED_LSB = *addr++;
    }
    
    LED = (uint16_t)addr;
} 

