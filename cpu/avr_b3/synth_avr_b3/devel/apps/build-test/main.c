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
    //if ('a' <= c && c <= 'z')
    //    c -= ('a'-'A');
    UDR0 = c + 1;
    sei();
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

/*
*   Test full duplex communications with a terminal emulator.
*       Test procedure:
*           1. Launch the terminal emulator at 115200 baud.
*           2. Type characters and ensure that they are echoed back.
*/
void test_interrupt(void)
{
    stdout = &mystdout;
    
    // set the receiver interrupt enable bit in the uart status reg B
    UCSRB0 |= (1<<RXCIE);

    // enable global interrupts
    sei();

    printf("Type characters and ensure they are echoed.\r\n");
        
    while ( 1 )
    {
        // flash the LSBit of the LEDs every second to signal that the test in running
        // and to type chars at the terminal emulation app
        LED_LSB ^= 0x01;
        msleep(500);
    }
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
    test_interrupt();
    //test_io();
    //test_sound();
    //test_out4();
    //test_vgaterm();
    
    return(0);
}

