/*
 * symtab.h -- definitions for symbol table
 */

#include <stdbool.h>
#include <inttypes.h>

#define SYMNAME(symbol) symtab[(symbol)].name
#define SYMVAL(symbol)  symtab[(symbol)].value

#define NAME_LEN 80
typedef uint8_t SymbolID;
typedef struct Symbol
{
    char        name[NAME_LEN];
    int         value;
    SymbolID    next;
} Symbol;

extern Symbol symtab[];

SymbolID SymLookup(const char *name);
SymbolID SymFind(const char *name);

