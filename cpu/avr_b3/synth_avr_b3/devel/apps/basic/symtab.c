/*
 *  symtab.c -- symbol table
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symtab.h"
#include "lexer.h"

extern char errorStr[];

// symbol table
static Symbol *symtab = NULL;

static char* strsave(const char* s)
{
    char* cp = calloc(strlen(s)+1, 1);
    if (cp)
    {
        strcpy(cp, s);
    }
    
    return cp;
}
  
static Symbol *SymCreate(const char *name)
{
    // create a new symbol
    Symbol *newEntry = (Symbol *)calloc(1, sizeof(Symbol));
    
    if (newEntry)
    {
        // set the name and add it to the front of the symbol list
        if ((newEntry->name = strsave(name)) == NULL)
            return NULL;
        newEntry->next = symtab;
        symtab = newEntry;
    }
     
    return newEntry;
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

bool SymLookup(int token)
{
    // if the symbol doesn't exist create one and fail if it can't
    if ((lexval.lexsym = SymFind(tokenStr)) == NULL)
        lexval.lexsym = SymCreate(tokenStr);
    if (!lexval.lexsym)
    {
        strcpy(errorStr, "memory allocation error");
        return false;
    }
                
    // for constants or literal strings assign the symbol's name as the lexeme
    switch (token)
    {
        case Constant:
        case String:
            lexval.lexeme = lexval.lexsym->name;
            break;
    }  
    
    return true; 
}

void FreeSymbol(Symbol *symbol)
{
    if (symbol->next)
        FreeSymbol(symbol->next);
    free(symbol);
}

void FreeSymtab(void)
{
    if (symtab)
        FreeSymbol(symtab);
    symtab = NULL;
}

// end of symtab.c

