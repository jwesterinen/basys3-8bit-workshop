/*
*   This is the main module for the a Basic interpreter simulator.
*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "symtab.h"
#include "ir.h"
#include "parser.h"
#include "runtime.h"

char message[80];
char *versionStr = "v3.0";
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

int indent = 0;
bool broMode = false;

void PrintNode(Node *node)
{
    if (node == NULL)
    {
        return;
    }
    for (int i = 0; i < indent; i++)
    {
        printf("     ");
    }
    printf("%s", NodeTypeStr(NODE_TYPE(node)));
    if (node->son)
    {
        puts("");
        PrintNode(node->son);
        if (node->son->bro)
        {
            indent++;
            PrintNode(node->son->bro);
            puts("");
            indent--;
        }
    }
}

void PrintExprTree(Node *root)
{
    if (root)
    {
        printf("expr tree:\n");
        PrintNode(root);
        puts("");
    }
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

