/*
 *  symtab.c -- symbol table
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include "symtab.h"
#include "lexer.h"
#include "main.h"

extern void Panic(const char *message);

#define SYM_NUMVAL(symbol)              ((symbol)->scalerVal.numval)
#define SYM_NUMVAL_IDX(symbol, index)   ((symbol)->vectorVal.numvals[(index)])
#define SYM_STRVAL(symbol)              ((symbol)->scalerVal.strval)
#define SYM_STRVAL_IDX(symbol, index)   ((symbol)->vectorVal.strvals[(index)])

extern char errorStr[];
char *emptyStr = "";

// dynamic memory management
struct memAllocEntry
{
    void *ptr;
    unsigned byteQty;
};
struct memAllocEntry memAllocList[100];
int memAllocListIdx = 0;
unsigned memAllocByteQty = 0;

uint16_t netAllocs = 0;

#ifdef DEBUG_ALLOCS
void *LocalCalloc(size_t nmemb, size_t size)
{
    netAllocs++;
    //sprintf(message, "allocation, net allocations = %d\n", netAllocs);
    //Console(message);
    Leds(netAllocs);
    return calloc(nmemb, size);
}

void LocalFree(void *ptr)
{
    netAllocs--;
    //sprintf(message, "free, net allocations = %d\n", netAllocs);
    //Console(message);
    Leds(netAllocs);
    free(ptr);
}

unsigned GetAllocMemQty(void)
{
    return netAllocs;
}

void ClearAllocMemQty(void)
{
    netAllocs = 0;
}
#endif

// symbol table
static Symbol *symtab = NULL;

static char* strsave(const char* s)
{
    char* cp = (char *)calloc(strlen(s)+1, 1);
    
    if (cp)
    {
        strcpy(cp, s);
    }
    else
    {
        Panic("system error: memory allocation error while creating string\n");
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
        newEntry->name = strsave(name);
        newEntry->next = symtab;
        symtab = newEntry;
    }
    else
    {
        Panic("system error: memory allocation error while creating symbol\n");
    }
     
    return newEntry;
}

struct Symbol *SymFind(const char *name)
{
    Symbol *next;
    
    // search symtab until match of end of symtab chain
    for (next = symtab; next; next = next->next)
    {
        if (!strcmp(next->name, name))
        {
            break;
        }
    }
   
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
            {
                break;
            }
            lexval.lexsym = SymCreate(tokenStr);
            lexval.lexsym->type = ST_NUMVAR;
            for (int i = 0; i < ARRAY_MAX; i++)
            {
                lexval.lexsym->value.numvals[i] = 0;
            }
            break;
        case Strvar:
            if ((lexval.lexsym = SymFind(tokenStr)))
            {
                break;
            }
            lexval.lexsym = SymCreate(tokenStr);
            lexval.lexsym->type = ST_STRVAR;
            for (int i = 0; i < ARRAY_MAX; i++)
            {
                lexval.lexsym->value.strvals[i] = emptyStr;
            }
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

// linearize the indeces into a 1D array
//   - the array is dimensioned as a(O,P,M,N)
//   - the array is referenced as a(o,p,m,n)
//   - the 1D index is calculated as follows:
//       - index = o*(P*M*N) + p*(M*N) + m*N + n
//   - dimSizes contain the dimensioned values:
//       - O = dimSizes[0], P = dimSizes[1], M = dimSizes[2], N = dimSizes[3], 
//   - using the indeces array:
//       - o = indeces[0], p = indeces[1], m = indeces[2], n = indeces[3]
//   - using these definitions, the resulting 1D index is:
//       - (the sum from i = 0 to 2 of indeces[i] * dimSizes[i+1]) + indeces[3]
int CalcVarIndex(Symbol *varsym, float indeces[4])
{
    int index = 0, i;
    
    if (SYM_DIM(varsym) > 0)
    {    
        // test for indeces in range
        for (i = 0; i < DIM_MAX; i++)
        {
            if (indeces[i] != 0)
            {
                if (indeces[i] >= SYM_DIMSIZES(varsym, i))
                {
                    strcpy(errorStr, "index out of range");
                    return -1;
                }
            }
        }
        
        // TODO: need to come up with an algorithm for this
        // transform n-dim indeces to 1D index
        index += indeces[0] * SYM_DIMSIZES(varsym, 1) *SYM_DIMSIZES(varsym, 2) * SYM_DIMSIZES(varsym, 3);
        index += indeces[1] * SYM_DIMSIZES(varsym, 2) *SYM_DIMSIZES(varsym, 3);
        index += indeces[2] * SYM_DIMSIZES(varsym, 3);
        index += indeces[3];
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
    *value = varsym->value.numvals[index];
    
    return true;
}

bool SymWriteNumvar(Symbol *varsym, float indeces[4], float value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    varsym->value.numvals[index] = value;
    
    return true;
}

bool SymReadStrvar(Symbol *varsym, float indeces[4], char **value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    *value = varsym->value.strvals[index];
    
    return true;
}

bool SymWriteStrvar(Symbol *varsym, float indeces[4], char *value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    varsym->value.strvals[index] = value;
    
    return true;
}

// end of symtab.c

