/*
*   This is the main module for the a Basic interpreter retro computer.
*/

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_console.h"
#include "parser.h"

#define USE_CONSOLE_KB

#ifdef USE_CONSOLE_KB

#define CR      '\r'    // newlines are returned as carriage return (CR) by terminal emulators
#define BS      0x7f    // backspaces are returned as delete (DEL) by terminal emulators

char keycode = 0;
#define GetKey() keycode
    
#else // default is PS2 keyboard

#define CR      '\n'
#define BS      '\b'

uint16_t keycode;
char GetKey(void)
{
    return (keycode = getps2());
}
    
#endif


// UART receive ISR
ISR(_VECTOR(3))
{
    // load the keyboard buffer with the char received by the UART
    keycode = UDR0;
    sei();
}

// PS2 receive ISR
ISR(_VECTOR(2))
{
    // PS2 HW API to get a ps2_keycode when there is a keypress
    getkey();
    sei();
}

char resultStr[80];
extern char errorStr[80];

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
    
    VgaReset();
    VgaPrintStr("AVR_B3 Basic Interpreter\n\n");
    VgaPrintStr(promptStr);
    while (1)
    {
        //if (keycode)
        if (GetKey())
        {
            //printf("keycode: %02x\r\n", keycode);
            
            // Enter
            if (keycode == CR)
            {
                commandBuf[i] = (uint8_t)'\0';
                i = 0;
                VgaNewline();
                if (ProcessCommand(commandBuf))
                {
                    if (resultStr[0] != '\0')
                    {
                        VgaPrintStr(resultStr);
                        resultStr[0] = '\0';
                        VgaNewline();
                    }
                }
                else
                {
                    VgaPrintStr(errorStr);
                    VgaNewline();
                }
                VgaPrintStr("ready\n");
                VgaPrintStr(promptStr);
                //printf("VGA_ROW_OFFSET = %d\r\n", VGA_ROW_OFFSET);
            }

            // Backspace
            else if (keycode == BS)
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
            else if (0x20 <= keycode && keycode <= 0x7f)
            {
                commandBuf[i++] = keycode;
                VGA_CHAR = keycode;
            }
            
            // clear the old keycode -- really only necessary for the console
            keycode = 0x00;
        }
        
#ifdef USE_CONSOLE_KB
        // required when using the console KB
        msleep(50);
#endif        
    }
}


