/*
 *  symtab.c -- symbol table
 */

#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
#include "message.h"
#include "symtab.h"

extern int asm_pass;
extern unsigned short cur_addr;

struct Symbol* symbolTable;
struct Symbol* symtabHead;
struct Symbol* symtabTail;

static char* strsave(const char* s)
{
    char* cp = calloc(strlen(s)+1, 1);
    if (cp)
    {
        strcpy(cp, s);
        return cp;
    }
    
    return (char*)NULL;
}
  
struct Symbol *s_create(char *name)
{
    struct Symbol *newSymbol = (struct Symbol *)NULL; 
       
    // ensure the symbol is unique then create it
    if (!s_find(name))
    {
        newSymbol = (struct Symbol *)calloc(1, sizeof(struct Symbol));    
        if (newSymbol)
        {
            newSymbol->s_name = strsave(name);
            newSymbol->s_type = ST_UNDEF;
            newSymbol->s_value = 0;
            newSymbol->s_next = (struct Symbol*)NULL;
            
            if (symbolTable == (struct Symbol*)NULL)
            {
                // init the symbol table
                symbolTable = newSymbol;
                symtabHead = symbolTable;
                symtabTail = symbolTable;
            }
            else
            {
                // add the new symbol to the end of the table
                symtabTail->s_next = newSymbol;
                symtabTail = newSymbol;
            }
        }
    }
     
    return newSymbol;
}

struct Symbol *s_find(const char *name)
{
    struct Symbol *ptr;
    
    // search symtab for name
    for (ptr = symtabHead; ptr; ptr = ptr->s_next)
    {
        if (strcmp(ptr->s_name, name) == 0)
        {
            break;                
        }
    }
   
    return ptr;
}

// symbol table lookup used in lexer
void s_lookup(int yylex)
{
    extern char *yytext;

    switch (yylex)
    {
        case Identifier:
            if ((yylval.y_sym = s_find(yytext)))
            {
                break;
            }
            yylval.y_sym = s_create(yytext);
            break;
        default:
            bug("s_lookup");
    }    
}

// define symbol
int s_define(struct Symbol *label, int type, unsigned value)
{
    if (label->s_type != ST_UNDEF)
    {
        error("label %s has already been defined", label->s_name);
        return 0;
    }
    if (value > 0xffff)
    {
        error("attemp to define label %s with a value that is too large to fit in 16 bits", label->s_name);
        return 0;
    }
    label->s_type = type;
    label->s_value = (unsigned short)value;
    return 1;
}

int chk_identifier(struct Symbol *symbol, int type)
{
    if ((symbol->s_type & type) == 0)
    {
        switch(type)
        {
            case ST_ID: 
                error("%s has not been defined as an ID", symbol->s_name); 
                break;
            case ST_LABEL: 
                error("%s has not been defined as a label", symbol->s_name); 
                break;
            case ST_ZPRAM_ADDR: 
                error("%s has not been defined as a zero page RAM offset", symbol->s_name); 
                break;
            case ST_RAM_ADDR: 
                error("%s has not been defined as a RAM address", symbol->s_name); 
                break;
            default:
                error("%s is undefined", symbol->s_name); 
                break;
        }
        return 0;
    }
    return 1;
}

// RAM allocation
#define FIRST_ZPRAM_ADDR 0x00
#define MAX_ZPRAM_ADDR 0xff
#define FIRST_RAM_ADDR 0x0100;
#define MAX_RAM_ADDR 0x0dff

int s_alloc_ram(struct Symbol *label, int type)
{
    static unsigned short nextZPAddr = FIRST_ZPRAM_ADDR;
    static unsigned short nextAddr = FIRST_RAM_ADDR;
    int retval = 0;

    switch(type)
    {
        case ST_ZPRAM_ADDR:
            // the symbol represents an address which needs to be allocated from ZP RAM
            if (nextZPAddr < MAX_ZPRAM_ADDR)
            {
                if ((retval = s_define(label, ST_ZPRAM_ADDR, nextZPAddr)))
                {
                    nextZPAddr++;
                }
            }
            else
            {
                error("max number of zero page variable has been reached");
            }
            break;
        case ST_RAM_ADDR:
            // the symbol represents an address which needs to be allocated from non-ZP RAM
            if (nextAddr < MAX_RAM_ADDR)
            {
                if ((retval = s_define(label, ST_RAM_ADDR, nextAddr)))
                {
                    nextAddr++;
                }
            }
            else
            {
                error("max number of variables has been reached");
            } 
            break;
    } 
    
    return retval;       
}

// end of symtab.c

