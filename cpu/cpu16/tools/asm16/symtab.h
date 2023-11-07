/*
 * symtab.h -- definitions for symbol table
 */

#define ST_UNDEF        0x0001
#define ST_ID           0x0002
#define ST_LABEL        0x0004
#define ST_ZPRAM_ADDR   0x0008
#define ST_RAM_ADDR     0x0010
#define ST_HEAP_ADDR    0x0020

struct Symbol
{
    const char*     s_name;
    int s_type;
    unsigned short  s_value;
    struct Symbol*  s_next;
};


struct Symbol *s_create(char *name);
struct Symbol *s_find(const char *name);
void s_lookup(int yylex);
int s_define(struct Symbol *label, int type, unsigned value);
int chk_identifier(struct Symbol *symbol, int type);
int s_alloc_ram(struct Symbol *label, int type, int size);

