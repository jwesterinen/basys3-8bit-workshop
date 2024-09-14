/*
 * symtab.h -- definitions for symbol table
 */

#define DIM_MAX 4
#define ARRAY_MAX 200

#define SYM_NAME(symbol)                ((symbol)->name)
#define SYM_TYPE(symbol)                ((symbol)->type)
#define SYM_DIM(symbol)                 ((symbol)->dim)
#define SYM_DIMSIZES(symbol, index)     ((symbol)->dimSizes[(index)])

enum SYMTYPE {ST_NUMVAR, ST_STRVAR, ST_FCT};
typedef struct Symbol
{
    char *name;
    enum SYMTYPE type;
    union 
    {
        float numval[ARRAY_MAX];
        char *strval[ARRAY_MAX];
    } value;
    float dim;                  // the dimension of the array, e.g. dim a(2,3,4) dim = 3
    float dimSizes[DIM_MAX];    // the size of each dimension, e.g. dim a(2,3,4) dimSizes = {2,3,4,0}
    struct Symbol *next;
} Symbol;

bool SymLookup(int token);
Symbol *SymFind(const char *name);
void FreeSymtab(void);
bool SymReadNumvar(Symbol *varsym, float indeces[4], float *value);
bool SymWriteNumvar(Symbol *varsym, float indeces[4], float value);
bool SymReadStrvar(Symbol *varsym, float indeces[4], char **value);
bool SymWriteStrvar(Symbol *varsym, float indeces[4], char *value);

