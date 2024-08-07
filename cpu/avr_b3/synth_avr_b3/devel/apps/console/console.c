/*
*   console.c
*   
*   This describes the canonical way to communicate from the console device to 
*   the VGA display.
*/

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_console.h"

#define CR      0x0d
#define BS      0x7f
#define SPACE   0X20

char kbBuf;

int Parse(const char* text)
{
    stdout = &mystdout;
    
    printf("line entered: %s\r\n", text);
    return 0;
}

// UART receive ISR
ISR(_VECTOR(3))
{
    // load the keyboard buffer with the char received by the UART
    kbBuf = UDR0;
    sei();
}

int main(void)
{
    // set UART baud rate to 115200
    UBRR0 = 54-1;

    char lineBuf[80];
    unsigned i = 0;
    
    // enable UART receiver interrupts
    UCSRB0 |= (1<<RXCIE);

    // enable global interrupts
    sei();

    // prompt string
    char *promptStr = "> ";
    
    // init the cursor to the bottom left
    VgaClearFrameBuffer();
    VGA_CUR_ROW = VGA_ROW_MAX;
    VGA_ROW_OFFSET = VGA_ROW_MIN;
       
    stdout = &mystdout;
    printf("starting console...\r\n");
    
    kbBuf = 0x00;
    VgaPrintStr("> ");
    while (1)
    {
        if (kbBuf)
        {
            //printf("keycode: %02x\r\n", kbBuf);
            
            // Enter
            if (kbBuf == CR)
            {
                lineBuf[i] = (uint8_t)'\0';
                i = 0;
                Parse(lineBuf);
                VgaNewline();
                VgaPrintStr(promptStr);
                //printf("VGA_ROW_OFFSET = %d\r\n", VGA_ROW_OFFSET);
            }

            // Backspace
            else if (kbBuf == BS)
            {
                if (VGA_CUR_COL > VGA_COL_MIN + strlen(promptStr))
                {          
                    VGA_CUR_COL--;
                    VGA_CHAR = ' ';
                    VGA_CUR_COL--;
                    i--;
                }
            }

            // display only printable characters
            else if (SPACE <= kbBuf && kbBuf <= BS)
            {
                lineBuf[i++] = kbBuf;
                VGA_CHAR = kbBuf;
            }
            
            kbBuf = 0x00;
        }
        msleep(50);
    }
}


