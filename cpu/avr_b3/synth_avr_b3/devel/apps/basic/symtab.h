/*
 * symtab.h -- definitions for symbol table
 */

#define SYM_NAME(symbol)                ((symbol)->name)
#define SYM_TYPE(symbol)                ((symbol)->type)
#define SYM_DIM(symbol)                 ((symbol)->dim)
#define SYM_DIMSIZES(symbol, index)     ((symbol)->dimSizes[(index)])
#define SYM_SIZE(symbol)                ((symbol)->size)

//enum SYMTYPE {ST_NUMVAR, ST_STRVAR, ST_FCT, ST_CONSTANT, ST_STRING};
enum SYMTYPE {ST_NUMVAR, ST_STRVAR, ST_FCT};
typedef struct Symbol
{
    char *name;
    enum SYMTYPE type;
    union 
    {
        float numval[100];
        char *strval[100];
    } value;
    float dim;                // the dimension of the array, e.g. dim a(2,3,4) dim = 3
    float dimSizes[4];        // the size of each dimension, e.g. dim a(2,3,4) dimSizes = {2,3,4,0}
    float size;
    struct Symbol *next;
} Symbol;

bool SymLookup(int token);
Symbol *SymFind(const char *name);
void FreeSymtab(void);
bool SymReadNumvar(Symbol *varsym, float indeces[4], float *value);
bool SymWriteNumvar(Symbol *varsym, float indeces[4], float value);
bool SymReadStrvar(Symbol *varsym, float indeces[4], char **value);
bool SymWriteStrvar(Symbol *varsym, float indeces[4], char *value);

