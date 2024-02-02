#define		F_CPU		50000000UL

#include <inttypes.h>
#include <util/delay.h>
#include "../include/system_avr_b3.h"

//static inline void msleep(uint16_t msec)
void msleep(uint16_t msec)
{
    while (msec)
    {	
        _delay_loop_2((uint32_t)F_CPU/4000UL);
        msec--;
    }
}

int main(void)
{
    while (1)
    {
        LED_LSB = 0x00;	
        msleep(500);
        LED_LSB = 0xff;	
        msleep(500);
    }

    return(0);
}

