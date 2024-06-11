/*
*   This is the main module for a Basic interpreter retro computer.
*/

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_console.h"
#include "runtime.h"

// default is PS2 keyboard
#define USE_CONSOLE_KB

#ifdef USE_CONSOLE_KB
    #define CR      '\r'    // newlines are returned as carriage return (CR) by terminal emulators
    #define BS      0x7f    // backspaces are returned as delete (DEL) by terminal emulators

    char keycode = 0;
    #define GetKey() keycode    
#else
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

#ifndef USE_CONSOLE_KB
// PS2 receive ISR
ISR(_VECTOR(2))
{
    // PS2 HW API to get a ps2_keycode when there is a keypress
    getkey();
    sei();
}
#endif

extern bool ready;

#define TABSIZE 4

void PrintResult(void)
{
    if (resultStr[0] != '\0')
    {
        VgaPrintStr(resultStr);
        VgaNewline();
        resultStr[0] = '\0';
    }
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
    
    //VgaReset();
    VgaPrintStr("AVR_B3 Basic Interpreter (dynamic symtab and node version)\n\n");
    VgaPrintStr("ready\n");
    VgaPrintStr(promptStr);    
    while (1)
    {
        if (GetKey())
        {
            // Enter
            if (keycode == CR)
            {
                commandBuf[i] = (uint8_t)'\0';
                i = 0;
                VgaNewline();
                if (!ProcessCommand(commandBuf))
                {
                    VgaPrintStr(errorStr);
                    VgaNewline();
                }
                if (ready)
                {
                    VgaPrintStr("ready\n");
                }
                VgaPrintStr(promptStr);        
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


