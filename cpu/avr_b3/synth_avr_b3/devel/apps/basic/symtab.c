/*
 *  symtab.c -- symbol table
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symtab.h"
#include "lexer.h"

extern void Panic(const char *message);

#define SYM_NUMVAL(symbol)              ((symbol)->scalerVal.numval)
#define SYM_NUMVAL_IDX(symbol, index)   ((symbol)->vectorVal.numvals[(index)])
#define SYM_STRVAL(symbol)              ((symbol)->scalerVal.strval)
#define SYM_STRVAL_IDX(symbol, index)   ((symbol)->vectorVal.strvals[(index)])

extern char errorStr[];
char *emptyStr = "";

// symbol table
static Symbol *symtab = NULL;

static char* strsave(const char* s)
{
    char* cp = calloc(strlen(s)+1, 1);
    
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
                break;
            lexval.lexsym = SymCreate(tokenStr);
            lexval.lexsym->type = ST_NUMVAR;
            lexval.lexsym->vectorVal.numvals = NULL;
            break;
        case Strvar:
            if ((lexval.lexsym = SymFind(tokenStr)))
                break;
            lexval.lexsym = SymCreate(tokenStr);
            lexval.lexsym->type = ST_STRVAR;
            lexval.lexsym->scalerVal.strval = emptyStr;
            lexval.lexsym->vectorVal.strvals = NULL;
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

bool SymConvertToArray(Symbol *varsym, int size)
{
    // allocate the array
    if (varsym->type == ST_NUMVAR)
    {
        if(varsym->vectorVal.numvals == NULL)
        {
            if ((varsym->vectorVal.numvals = (float *)calloc(size, sizeof(float))) == NULL)
            {
                Panic("system error: memory allocation error while dimensioning num array\n");
                return false;
            }
        }
    }
    else if (varsym->type == ST_STRVAR)
    {
        if (varsym->vectorVal.strvals == NULL)
        {
            if ((varsym->vectorVal.strvals = (char **)calloc(size, sizeof(char *))) == NULL)
            {
                Panic("system error: memory allocation error while dimensioning string array\n");
                return false;
            }
            
            // default all string array elements to the empty string
            for (int i = 0; i < size; i++)
            {
                varsym->vectorVal.strvals[i] = emptyStr;
            }
        }
    }
    
    return true;
}

void FreeSymbol(Symbol *symbol)
{
    if (symbol->next)
        FreeSymbol(symbol->next);
    if (symbol->type == ST_NUMVAR)
    {
        free (symbol->vectorVal.numvals);
    }
    else
    {
        free (symbol->vectorVal.strvals);
    }
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
    
    *value = (varsym->dim == 0) ? SYM_NUMVAL(varsym) : SYM_NUMVAL_IDX(varsym, index);
    
    return true;
}

bool SymWriteNumvar(Symbol *varsym, float indeces[4], float value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    
    if (varsym->dim == 0)
    {
        SYM_NUMVAL(varsym) = value;
    }
    else
    {
        SYM_NUMVAL_IDX(varsym, index) = value;
    }
    
    return true;
}

bool SymReadStrvar(Symbol *varsym, float indeces[4], char **value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    
    *value = (varsym->dim == 0) ? SYM_STRVAL(varsym) : SYM_STRVAL_IDX(varsym, index);
    
    return true;
}

bool SymWriteStrvar(Symbol *varsym, float indeces[4], char *value)
{
    int index = CalcVarIndex(varsym, indeces);
    
    if (index == -1)
    {
        return false;
    }
    
    if (varsym->dim == 0)
    {
        SYM_STRVAL(varsym) = value;
    }
    else
    {
        SYM_STRVAL_IDX(varsym, index) = value;
    }
    
    return true;
}

// end of symtab.c

