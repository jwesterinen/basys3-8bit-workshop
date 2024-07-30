/*
 * symtab.h -- definitions for symbol table
 */

#define SYM_NAME(symbol)                ((symbol)->name)
#define SYM_TYPE(symbol)                ((symbol)->type)
#define SYM_INTVAL(symbol)              ((symbol)->value.intval[0])
#define SYM_INTVAL_IDX(symbol, index)   ((symbol)->value.intval[(index)])
#define SYM_STRVAL(symbol)              ((symbol)->value.strval[0])
#define SYM_STRVAL_IDX(symbol, index)   ((symbol)->value.strval[(index)])
#define SYM_DIM(symbol)                 ((symbol)->dim)
#define SYM_DIMSIZES(symbol, index)     ((symbol)->dimSizes[(index)])
#define SYM_SIZE(symbol)                ((symbol)->size)

enum SYMTYPE {ST_INTVAR, ST_STRVAR, ST_FCT, ST_CONSTANT, ST_STRING};
typedef struct Symbol
{
    char *name;
    enum SYMTYPE type;
    union 
    {
        int intval[100];
        char *strval[100];
    } value;
    int dim;                // the dimension of the array
    int dimSizes[4];        // the size of each dimension, e.g. dim a(2,3,4) dimSizes = [2][3][4]
    int size;
    struct Symbol *next;
} Symbol;

bool SymLookup(int token);
Symbol *SymFind(const char *name);
void FreeSymtab(void);
bool SymReadIntvar(Symbol *varsym, int indeces[4], int *value);
bool SymWriteIntvar(Symbol *varsym, int indeces[4], int value);
bool SymReadStrvar(Symbol *varsym, int indeces[4], char **value);
bool SymWriteStrvar(Symbol *varsym, int indeces[4], char *value);

