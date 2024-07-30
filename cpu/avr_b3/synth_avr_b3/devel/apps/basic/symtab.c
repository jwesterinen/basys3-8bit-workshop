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
        {
            return NULL;
        }
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
    // variables default to scalers (dim=0)
    if ((lexval.lexsym = SymFind(tokenStr)) == NULL)
    {
        lexval.lexsym = SymCreate(tokenStr);
    }
    if (!lexval.lexsym)
    {
        strcpy(errorStr, "memory allocation error");
        return false;
    }

    // set the symbol type and for constants or literal strings assign the symbol's name as the lexeme
    switch (token)
    {  
        case Intvar:
            lexval.lexsym->type = ST_INTVAR;
            break;
        case Strvar:
            lexval.lexsym->type = ST_STRVAR;
            break;
        case Function:
            lexval.lexsym->type = ST_FCT;
            break;
        case Constant:
            lexval.lexsym->type = ST_CONSTANT;
            lexval.lexeme = lexval.lexsym->name;
            break;
        case String:
            lexval.lexsym->type = ST_STRING;
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

int CalcVarIndex(Symbol *varsym, int indeces[4])
{
    int index = 0;
    
    if (SYM_DIM(varsym) > 0)
    {
        if (indeces[0] < SYM_DIMSIZES(varsym, 0))
        {
            index = indeces[0];
        }
        else
        {
            return -1;
        }
        
        // linearize the indeces to index the value
        for (int i = 1; i < SYM_DIM(varsym); i++)
        {
            if (indeces[i] < SYM_DIMSIZES(varsym, i))
            {
                index += indeces[i] * SYM_DIMSIZES(varsym, i);
            }
            else
            {
                return -1;
            }
        }
    }
    
    return index;
}

bool SymReadIntvar(Symbol *varsym, int indeces[4], int *value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    
    *value = SYM_INTVAL_IDX(varsym, index);
    
    return true;
}

bool SymWriteIntvar(Symbol *varsym, int indeces[4], int value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    
    SYM_INTVAL_IDX(varsym, index) = value;
    
    return true;
}

bool SymReadStrvar(Symbol *varsym, int indeces[4], char **value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    
    *value = SYM_STRVAL_IDX(varsym, index);
    
    return true;
}

bool SymWriteStrvar(Symbol *varsym, int indeces[4], char *value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    
    SYM_STRVAL_IDX(varsym, index) = value;
    
    return true;
}

// end of symtab.c

