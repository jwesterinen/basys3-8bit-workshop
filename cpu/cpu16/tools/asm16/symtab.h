/*
 * symtab.h -- definitions for symbol table
 */

struct Symbol
{
    const char*     s_name;
    unsigned short  s_addr;
    struct Symbol*  s_next;
};
struct Symbol* symbolTable;
struct Symbol* symtabHead;
struct Symbol* symtabTail;

struct Symbol *s_create(char *name);
struct Symbol *s_find(const char *name);

