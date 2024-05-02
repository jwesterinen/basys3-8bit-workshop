/*
*   console.c
*/

#include <time.h>
#include <stdlib.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_lib.h"

void Console(void);

int Parse(const char* text)
{
    stdout = &mystdout;
    
    printf("line entered: %s\r\n", text);
    return 0;
}

int main(void)
{
    // set UART baud rate to 115200
    UBRR0 = 13-1;

    Console();
}
