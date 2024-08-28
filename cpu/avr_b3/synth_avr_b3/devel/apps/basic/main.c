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

char message[80];
char *versionStr = "v0.7";
char *promptStr = "> ";
    
extern bool ready;

// print a message to the console device
void Console(const char *string)
{
    stdout = &mystdout;
    printf(string);
}

// print out messages during runtime
void Message(const char *message)
{
    VgaPrintStr(message);
}

void PutString(char *string)
{
    VgaPrintStr(string);
}

// returns the next CR-terminated string from the input device
char *GetString(char *buffer)
{
    unsigned i = 0;
        
    while (1)
    {
        if (GetKey())
        {
            //sprintf(message, "keycode = %d\r\n", keycode);
            //Console(message);
            
            // Enter - terminate the command string and return
            if (keycode == CR)
            {
                buffer[i++] = keycode;
                buffer[i] = (uint8_t)'\0';
                VgaNewline();
                keycode = 0x00;
                return buffer;
            }

            // Backspace - move the cursor back on place stopping at the prompt
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
                buffer[i++] = keycode;
                VGA_CHAR = keycode;
            }
            
            // clear the old keycode -- really only necessary for the console
            keycode = 0x00;
        }
        msleep(50);
    }
}

uint8_t MemRead(uint16_t *addr)
{
    return (*(volatile uint8_t *)(addr));
}

void MemWrite(uint16_t *addr, uint8_t data)
{
    (*(volatile uint8_t *)(addr)) = data;
}

void Tone(uint16_t freq, uint16_t duration)
{
    KeyBeep(freq, duration);
}

void InitDisplay(void)
{
    VgaReset();
    PutString("AVR_B3 Basic Interpreter ");
    PutString(versionStr);
    PutString("\n\n");
}

void Delay(uint16_t duration)
{
    msleep(duration);
}

int main(void)
{
    // set UART baud rate to 115200
    //UBRR0 = 13-1;
    UBRR0 = 54-1;

    // enable UART receiver interrupts
    UCSRB0 |= (1<<RXCIE);

    // enable global interrupts
    sei();

    Console("starting basic interpreter...\r\n");
    
    char command[80];
    
    InitDisplay();
    while (1)
    {
        if (ready)
        {
            PutString("ready\n");
        }
        PutString(promptStr);        
        GetString(command);
        command[strlen(command)-1] = '\0';
        if (!ProcessCommand(command))
        {
            PutString(errorStr);
            PutString("\n");
        }
    }
}

// end of main.c

