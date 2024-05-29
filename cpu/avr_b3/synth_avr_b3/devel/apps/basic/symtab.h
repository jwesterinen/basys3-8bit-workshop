/*
 * symtab.h -- definitions for symbol table
 */

#include <stdbool.h>
#include <inttypes.h>

typedef uint8_t SymbolID;
typedef struct Symbol
{
    char        name[20];
    int         value;
    SymbolID    next;
} Symbol;

extern Symbol symtab[];

SymbolID SymLookup(const char *name);
SymbolID SymFind(const char *name);

