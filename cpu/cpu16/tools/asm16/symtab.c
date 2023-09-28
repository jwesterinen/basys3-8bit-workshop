/*
 *  symtab.c -- symbol table
 */

#include <stdlib.h>
#include <string.h>
#include "symtab.h"

extern int asm_pass;
extern unsigned short cur_addr;

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
       
    // extract the base symbol from the label, i.e. w/o the ending ':'
    name[strlen(name) - 1] = (char)0;
    
    // ensure the symbol is unique then create it
    if (!s_find(name))
    {
        newSymbol = (struct Symbol *)calloc(1, sizeof(struct Symbol));    
        if (newSymbol)
        {
            newSymbol->s_next = (struct Symbol*)NULL;
            newSymbol->s_name = strsave(name);
            newSymbol->s_addr = cur_addr;
            
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

// end of symtab.c

