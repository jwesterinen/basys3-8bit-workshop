#include <time.h>
#include <stdlib.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_lib.h"

// UART receive ISR
ISR(_VECTOR(3))
{
    uint8_t	c;
    c = UDR0;
    if ('a' <= c && c <= 'z')
        c -= ('a'-'A');
    UDR0 = c + 1;
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

void test_uart_isr(void)
{
    stdout = &mystdout;
    
    printf("enter characters...\r\n");
    while (1);
}

__attribute__((noinline)) void test_io(void)
{
    while ( 1 )
    {	
        // write switches to LEDs
        LED = SW;
        
        // select DP with switch LSN
        DP = ((SW_LSB & 0x0f) == 0x01) ? (1<<DP3) : 
             ((SW_LSB & 0x0f) == 0x02) ? (1<<DP2) : 
             ((SW_LSB & 0x0f) == 0x04) ? (1<<DP1) :
             ((SW_LSB & 0x0f) == 0x08) ? (1<<DP0) :
                                         (0     ) ;
        
        // write switch LSB_MSN to out4 pins
        //OUT4 = SW_LSB >> 4;
        
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

void test_sound(void)
{
    while (1)
    {
        LED = SW;
        MIXER = SW_MSB >> 4;
        switch (MIXER)
        {
            case VCO1:
                VCO1_FREQ = SW;   
                break; 
            case VCO2:
                VCO2_FREQ = SW;   
                break; 
            case NOISE:
                NOISE_FREQ = SW;   
                break; 
        }
    }
}

/*
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
*/

void test_vgaterm(void)
{
    int i;
    
    for (i = 0; i < (VGA_ROW_MAX+1)*(VGA_COL_MAX+1); i++)
        VGA_CHAR = i % 256;
}

int main(void)
{
    // set UART baud rate to 115200
    UBRR0 = 13-1;

    //test_printf();
    //test_interrupt();
    //test_uart_isr();
    //test_io();
    //test_sound();
    //test_out4();
    test_vgaterm();
    
    return(0);
}

