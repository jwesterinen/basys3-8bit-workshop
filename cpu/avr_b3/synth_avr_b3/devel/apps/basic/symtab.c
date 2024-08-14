/*
 *  symtab.c -- symbol table
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symtab.h"
#include "lexer.h"

#define SYM_NUMVAL(symbol)              ((symbol)->value.numval[0])
#define SYM_NUMVAL_IDX(symbol, index)   ((symbol)->value.numval[(index)])
#define SYM_STRVAL(symbol)              ((symbol)->value.strval[0])
#define SYM_STRVAL_IDX(symbol, index)   ((symbol)->value.strval[(index)])

extern char errorStr[];

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

// if the symbol doesn't exist create one and fail if it can't
// variables default to scalers (dim=0)
bool SymLookup(int token)
{
    // set the symbol type and for constants or literal strings assign the symbol's name as the lexeme
    switch (token)
    {  
        case Numvar:
            if ((lexval.lexsym = SymFind(tokenStr)))
                break;
            lexval.lexsym = SymCreate(tokenStr);
            lexval.lexsym->type = ST_NUMVAR;
            break;
        case Strvar:
            if ((lexval.lexsym = SymFind(tokenStr)))
                break;
            lexval.lexsym = SymCreate(tokenStr);
            lexval.lexsym->type = ST_STRVAR;
            break;
        case Function:
            if ((lexval.lexsym = SymFind(tokenStr)))
                break;
            lexval.lexsym = SymCreate(tokenStr);
            lexval.lexsym->type = ST_FCT;
            break;
        case Constant:
        case String:
            lexval.lexeme = strsave(tokenStr);
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

int CalcVarIndex(Symbol *varsym, float indeces[4])
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

bool SymReadNumvar(Symbol *varsym, float indeces[4], float *value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    
    *value = SYM_NUMVAL_IDX(varsym, index);
    
    return true;
}

bool SymWriteNumvar(Symbol *varsym, float indeces[4], float value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    
    SYM_NUMVAL_IDX(varsym, index) = value;
    
    return true;
}

bool SymReadStrvar(Symbol *varsym, float indeces[4], char **value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    
    *value = SYM_STRVAL_IDX(varsym, index);
    
    return true;
}

bool SymWriteStrvar(Symbol *varsym, float indeces[4], char *value)
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

