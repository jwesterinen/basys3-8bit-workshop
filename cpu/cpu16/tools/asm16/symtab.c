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
            newSymbol->s_defined = 0;
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
int s_define(struct Symbol *label, unsigned value)
{
    if (label->s_defined)
    {
        error("label %s has already been defined", label->s_name);
        return 0;
    }
    if (value > 0xffff)
    {
        error("attemp to define label %s with a value that is too large to fit in 16 bits", label->s_name);
        return 0;
    }
    label->s_value = (unsigned short)value;
    label->s_defined = 1;
    return 1;
}

int chk_identifier(struct Symbol *label)
{
    if (!label->s_defined)
    {
        error("label %s is undefined", label->s_name);
        return 0;
    }
    return 1;
}

// end of symtab.c

