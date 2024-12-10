/*
*   This is the main module for the a Basic interpreter.
*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "runtime.h"

char message[80];
char *versionStr = "v1.0";
char *promptStr = "> ";
char frameBuf[40][80];

// print a message to the console device
void Console(const char *string)
{
    printf("%s", string);
}

// print out messages during runtime controlled by verbosity
void Message(const char *message)
{
    puts(message);
}

// print out system error messages unconditionally during runtime
void Panic(const char *message)
{
    puts(message);
}

char *GetString(char *buffer)
{
    return fgets(buffer, 80, stdin);
}

void PutString(char *string)
{
    if (textMode)
    {
        printf("%s", string);
    }
}

uint8_t MemRead(uint16_t addr)
{
    printf("read from addr = %d\n", addr);
    return 55;
}

void MemWrite(uint16_t addr, uint8_t data)
{
    printf("write %d to addr %d\n", data, addr);
}

void Tone(uint16_t freq, uint16_t duration)
{
    printf("play tone at %d Hz for %d mSec\n", freq, duration);
}

uint16_t Switches(void)
{
    printf("read switches\n");
    return 1234;
}

uint8_t Buttons(void)
{
    printf("read buttons\n");
    return 56;
}

void Leds(uint16_t value)
{
    printf("show %d on the LEDs\n", value);
}

void Display7(uint16_t value, uint8_t displayQty)
{
    printf("display %d on %d 7-segment displays\n", value, displayQty);
}

void Delay(uint16_t duration)
{
    printf("delay %d mSec\n", duration);
}

void InitDisplay(void)
{
    PutString("AVR_B3 Basic Interpreter ");
    PutString(versionStr);
    PutString("\n\n");
}

uint8_t GfxGetChar(uint8_t row, uint8_t col)
{
    printf("read char from screen{%d,%d}\n", row, col);
    return frameBuf[row][col];
}

uint8_t GfxPutChar(uint8_t row, uint8_t col, uint8_t c)
{
    printf("write char '%d' to screen{%d,%d}\n", c, row, col);
    frameBuf[row][col] = c;
    return c;
}

void GfxClearScreen(void)
{
    printf("clear the screen\n");
}

void GfxTextMode(uint8_t mode)
{
    printf("text mode %s\n", (mode) ? "on" : "off");
}

bool SdMount(void)
{
    printf("mount SD card\n");
    return true;
}

bool SdUnmount(void)
{
    printf("unmount SD card\n");
    return true;
}

bool SdList(void)
{
    printf("list files on SD card\n");
    return true;
}

bool SdDelete(const char *filename)
{
    if (filename != NULL)
    {
        printf("delete file %s from SD card\n", filename);
        return true;
    }
    else
    {
        strcpy(errorStr, "missing filename");
        return false;
    }

    return true;
}

bool SdLoad(const char *filename)
{
    if (filename != NULL)
    {
        printf("load file %s from SD card\n", filename);
        return true;
    }
    else
    {
        strcpy(errorStr, "missing filename");
        return false;
    }

    return true;
}

bool SdSave(const char *filename)
{
     if (filename != NULL)
    {
        printf("save file %s to SD card\n", filename);
        return true;
    }
    else
    {
        strcpy(errorStr, "missing filename");
        return false;
    }

   return true;
}

int main(void)
{
    char command[80]; 

    InstallBuiltinFcts();
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
    
    return 0;
}

// end of main.c

