/*
 *   terminal.c
 */

#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_console.h"


// UART receive ISR
ISR(_VECTOR(3))
{
    uint8_t	c;

    c = UDR0;
    UDR0 = c;
    sei();
}


// PS2 receive ISR
ISR(_VECTOR(2))
{
    extern uint16_t ps2_keycode;
    uint8_t volatile *addr;

    getkey();

    
    addr = (volatile uint8_t *)(0x8004);
    *addr++ = (uint8_t) (ps2_keycode & 0xff);
    *addr = (uint8_t) ((ps2_keycode >> 8) & 0xff);

    //UDR0 = (uint8_t) (code & 0xff);

    sei();
}



int main(void)
{
    uint16_t  ps2keycode;

    stdout = &mystdout;
    // set the receiver interrupt enable bit in the uart status reg B
    UCSRB0 |= (1<<RXCIE);
    // set UART baud rate to 115200
    UBRR0 = 13-1;
    // enable global interrupts
    sei();

    printf("Type characters and ensure they are echoed.\r\n");

    while (1)
    {
        ps2keycode = getps2(); 
        if (ps2keycode != 0)
            printf("keycode = %04x\r\n", ps2keycode);
        msleep(5);
    }
}

