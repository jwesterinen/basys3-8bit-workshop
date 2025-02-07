/*
*   This is the main module for the a Basic interpreter simulator.
*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "symtab.h"
#include "expr.h"
#include "parser.h"
#include "runtime.h"

#define VGA_ROW_MAX         39
#define VGA_COL_MAX         79
typedef char VGA_DISPLAY_BUFFER[VGA_ROW_MAX][VGA_COL_MAX];

char message[80];
char *versionStr = "v4.0";
char *promptStr = "> ";
char frameBuf[40][80];

// display buffer used for animation
VGA_DISPLAY_BUFFER dispBuf;


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
    return 0;
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

uint8_t GfxPutChar(uint8_t row, uint8_t col, uint8_t c)
{
    printf("write char '%d' to screen{%d,%d}\n", c, row, col);
    frameBuf[row][col] = c;
    return c;
}

uint8_t GfxGetChar(uint8_t row, uint8_t col)
{
    printf("read char from screen{%d,%d}\n", row, col);
    return frameBuf[row][col];
}

uint8_t GfxPutDB(uint8_t row, uint8_t col, uint8_t c)
{
    dispBuf[row][col] = c;
    return dispBuf[row][col];
}

uint8_t GfxGetDB(uint8_t row, uint8_t col)
{
    return dispBuf[row][col];
}

void GfxLoadFB(void)
{
    for (int row = 0; row <= VGA_ROW_MAX; row++)
        for (int col = 0; col <= VGA_COL_MAX; col++)
            frameBuf[row][col] = dispBuf[row][col];
}

void GfxClearScreen(void)
{
    for (int row = 0; row <= VGA_ROW_MAX; row++)
        for (int col = 0; col <= VGA_COL_MAX; col++)
            frameBuf[row][col] = 0x20;
}

void GfxClearDB()
{
    for (int row = 0; row <= VGA_ROW_MAX; row++)
        for (int col = 0; col <= VGA_COL_MAX; col++)
            dispBuf[row][col] = 0x20;
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

char *NodeTypeStr(enum NodeType type)
{
    switch (type)
    {
        case NT_NONE:
            return "none";
        case NT_EXPR:
            return "EXPR ";
        case NT_LOGIC_EXPR:
            return "LE   ";
        case NT_LOGIC_EXPR_PRIME:
            return "LE'  ";
        case NT_REL_EXPR:
            return "RE   ";
        case NT_REL_EXPR_PRIME:
            return "RE'  ";
        case NT_SHIFT_EXPR:
            return "SE   ";
        case NT_SHIFT_EXPR_PRIME:
            return "SE'  ";
        case NT_ADD_EXPR:
            return "AE   ";
        case NT_ADD_EXPR_PRIME:
            return "AE'  ";
        case NT_MULT_EXPR:
            return "ME   ";
        case NT_MULT_EXPR_PRIME:
            return "ME'  ";
        case NT_UNARY_EXPR:
            return "UE   ";
        case NT_POSTFIX_EXPR:
            return "PFE  ";
        case NT_SUB_EXPR_LIST:
            return "SEL  ";
        case NT_PRIMARY_EXPR:
            return "PE   ";
        case NT_BINOP:
            return "BOP  ";
        case NT_UNOP:
            return "UNOP ";
        case NT_CONSTANT:
            return "CNST ";
        case NT_NUMVAR:
            return "NVAR ";
        case NT_STRVAR:
            return "SVAR ";
        case NT_FCT:
            return "FCT  ";
        case NT_STRING:
            return "STR  ";
    }
    return "unknown type";
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

