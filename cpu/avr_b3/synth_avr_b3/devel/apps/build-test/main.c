#include "../../include/avr_b3.h"
#include "../../include/avr_b3_lib.h"

// UART receive ISR
ISR(_VECTOR(3))
{
    uint8_t	c;
    c = UDR0;
    if ('a' <= c && c <= 'z')
        c -= ('a'-'A');
    UDR0 = c;
}

// timer0 ISR
ISR(_VECTOR(1))
{
    TCR0 = TCR0;
    LED_LSB ^= 0x02;
}

void test_printf(void)
{
    int	i;

    stdout = &mystdout;

    i = 0;
    LED_LSB = 0x90;
    LED_LSB ^= 0x03;

    while (1)
    {	
        LED_LSB ^= 0x03;
        printf("[x] %d => %d\r\n", i, i*i);
        msleep(1000);
        i++;
    }
}

void test_interrupt(void)
{
    UCSRB0 |= (1<<RXCIE);

    TCR0 = 0x02;
    TCR0 |= (1<<TOFIE);

    sei();
    while (1)
    {	
        LED_LSB ^= 0x01;
        msleep(500);
    }
}

void test_io(void)
{
    while ( 1 )
    {	
        // write switches to LEDs
        LED = SW;
        
        // write keypad to display 0
        DISPLAY0 = KEYPAD & 0x0f;
        
        // write buttons to display 1-3
        DISPLAY1 = BUTTONS;
        DISPLAY2 = BUTTONS;
        DISPLAY3 = BUTTONS;
    }
}

int main(void)
{
    // set UART baud rate to 115200
    UBRR0 = 13-1;

    //test_printf();
    //test_interrupt();
    test_io();

    return(0);
}

