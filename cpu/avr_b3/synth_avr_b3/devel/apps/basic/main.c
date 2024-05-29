/*
*   This is the main module for the a Basic interpreter retro computer.
*/

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_lib.h"
#include "parser.h"

#define CR      0x0d
#define BS      0x7f
#define SPACE   0X20

char kbBuf;

char resultStr[80];
extern char errorStr[80];
    
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
    UBRR0 = 13-1;

    char commandBuf[80];
    unsigned i = 0;
    
    // enable UART receiver interrupts
    UCSRB0 |= (1<<RXCIE);

    // enable global interrupts
    sei();

    // prompt string
    char *promptStr = "> ";
    
    stdout = &mystdout;
    printf("starting basic interpreter...\r\n");
    
    kbBuf = 0x00;
    VgaClearFrameBuffer();
    VgaPrintStr("AVR_B3 Basic Interpreter\n\n");
    VgaPrintStr(promptStr);
    while (1)
    {
        if (kbBuf)
        {
            //printf("keycode: %02x\r\n", kbBuf);
            
            // Enter
            if (kbBuf == CR)
            {
                commandBuf[i] = (uint8_t)'\0';
                i = 0;
                if (ProcessCommand(commandBuf))
                {
                    if (resultStr[0] != '\0')
                    {
                        VgaNewline();
                        VgaPrintStr(resultStr);
                        resultStr[0] = '\0';
                    }
                }
                else
                {
                    VgaNewline();
                    VgaPrintStr(errorStr);
                }
                VgaNewline();
                VgaPrintStr("ready\n");
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
                commandBuf[i++] = kbBuf;
                VGA_CHAR = kbBuf;
            }
            
            kbBuf = 0x00;
        }
        msleep(50);
    }
}


