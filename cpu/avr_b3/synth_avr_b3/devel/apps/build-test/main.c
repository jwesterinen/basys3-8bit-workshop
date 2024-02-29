#include <inttypes.h>
#include <time.h>
#include <stdlib.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_lib.h"
#include "../../include/avr_b3_stdio.h"

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

__attribute__((noinline)) void test_io(void)
{
    while ( 1 )
    {	
        // write switch MSBs to LED MSBs
        LED_MSB = SW_MSB;
        
        // select DP with switch LSN
        DP = ((SW_LSB & 0x0f) == 0x01) ? (1<<DP3) : 
             ((SW_LSB & 0x0f) == 0x02) ? (1<<DP2) : 
             ((SW_LSB & 0x0f) == 0x04) ? (1<<DP1) :
             ((SW_LSB & 0x0f) == 0x08) ? (1<<DP0) :
                                         (0     ) ;
        
        // write switch LSB_MSN to out4 pins
        OUT4 = SW_LSB >> 4;
        
        // set display control with SW15
        DISPCTRL = (SW >> 15) & 0x0001;
    
        // write keypad to display 0
        DISPLAY0 = KEYPAD & 0x0f;
        
        // write buttons to display 1-3
        DISPLAY1 = BUTTONS;
        DISPLAY2 = BUTTONS;
        DISPLAY3 = BUTTONS;
    }
}

__attribute__((noinline)) void test_out4(void)
{
    stdout = &mystdout;
    srand(time(NULL));
    int r;
    
    while (1)
    {
        r = rand() % 0xf;
        OUT4 = r;
        printf("random value = %x\r\n", r);
        msleep(100);
    }
}

int main(void)
{
    // set UART baud rate to 115200
    UBRR0 = 13-1;

    //test_printf();
    //test_interrupt();
    //test_io();
    test_out4();
    
    return(0);
}

