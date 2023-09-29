/*
 * symtab.h -- definitions for symbol table
 */

struct Symbol
{
    const char*     s_name;
    int             s_defined;
    unsigned short  s_value;
    struct Symbol*  s_next;
};

struct Symbol *s_create(char *name);
struct Symbol *s_find(const char *name);
void s_lookup(int yylex);
int s_define(struct Symbol *label, unsigned short value);
int chk_identifier(struct Symbol *label);

