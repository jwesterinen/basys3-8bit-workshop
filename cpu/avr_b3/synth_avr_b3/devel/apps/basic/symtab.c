/*
 *  symtab.c -- symbol table
 */

#include <stdlib.h>
#include <string.h>
#include "symtab.h"

#define SYMTAB_SIZE 40

// symbol table
Symbol symtab[SYMTAB_SIZE];
int symtabIdx = 1;

static SymbolID SymCreate(const char *name)
{
    SymbolID newSymbol = symtabIdx++;    
    strcpy(symtab[newSymbol].name, name);
    symtab[newSymbol].value = 0;
     
    return newSymbol;
}

SymbolID SymLookup(const char *name)
{
    SymbolID symbol = SymFind(name);    
    if (symbol == 0) 
        symbol = SymCreate(name);
        
    return symbol;
}

SymbolID SymFind(const char *name)
{
    // search symtab until match of end of symtab chain
    for (int i = 0; i < SYMTAB_SIZE; i++)
        if (strcmp(symtab[i].name, name) == 0)
            return i;
   
    return 0;
}

// end of symtab.c

