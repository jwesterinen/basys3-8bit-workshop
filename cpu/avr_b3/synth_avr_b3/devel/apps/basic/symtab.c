/*
 *  symtab.c -- symbol table
 */

#include <stdlib.h>
#include <string.h>
#include "symtab.h"

// symbol table
static Symbol *symtab = NULL;

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
  
static Symbol *SymCreate(const char *name)
{
    // create a new symbol
    Symbol *newEntry = (Symbol *)calloc(1, sizeof(Symbol));
    
    if (newEntry)
    {
        // set the name and add it to the front of the symbol list
        newEntry->name = strsave(name);
        newEntry->next = symtab;
        symtab = newEntry;
    }
     
    return newEntry;
}

Symbol *SymLookup(const char *name)
{
    Symbol *symbol;

    if ((symbol = SymFind(name)) == NULL)
        symbol = SymCreate(name);
        
    return symbol;
}

struct Symbol *SymFind(const char *name)
{
    Symbol *next;
    
    // search symtab until match of end of symtab chain
    for (next = symtab; next; next = next->next)
        if (!strcmp(next->name, name))
            break;
   
    return next;
}

// end of symtab.c

