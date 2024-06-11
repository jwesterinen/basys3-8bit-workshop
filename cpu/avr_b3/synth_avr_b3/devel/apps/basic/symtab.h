/*
 * symtab.h -- definitions for symbol table
 */

#include <stdbool.h>
#include <inttypes.h>

#define SYM_NAME(symbol) symbol->name
#define SYM_TYPE(symbol)  symbol->type
#define SYM_INTVAL(symbol)  symbol->value.intval
#define SYM_STRVAL(symbol)  symbol->value.strval

enum SYMTYPE {ST_INTVAR, ST_STRVAR};
typedef struct Symbol
{
    char *name;
    enum SYMTYPE type;
    union 
    {
        int intval;
        char *strval;
    } value; 
    struct Symbol *next;
} Symbol;

Symbol *SymLookup(const char *name);
Symbol *SymFind(const char *name);

