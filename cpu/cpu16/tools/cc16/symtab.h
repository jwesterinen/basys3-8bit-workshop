/*
 * symtab.h -- definitions for symbol table
 */

struct Symtab
{
    char   *s_name;             // name
    int     s_type;             // symbol type
    int     s_blknum;           // static block depth
    union 
    {
        int s__num;
        struct Symtab *s__link;
    } s__; 
    int     s_offset;           // symbol definition   
    int     s_size;             // size of the variable, 1 for normal vars, n for arrays
    int     s_ref_level;        // reference level, 0 for scalars, +1 for each pointer level
    struct Symtab *s_next;      // next entry
};

#define s_pnum  s__.s__num      // count of parameters
#define NOT_SET (-1)            // no count yet set
#define s_plist s__.s__link     // chain of parameters

// s_type values
#define UDEC    0               // not declared
#define FUNC    1               // function
#define VFUNC   2               // void function
#define UFUNC   3               // undefined function
#define VAR     4               // declared variable
#define PARM    5               // undeclared parameter

// s_type string values
#define SYMmap   "udecl", "fct", "vfct", "udef fct", "var", "parm"

// offsets
extern int g_offset, l_offset, l_max;

// reference level
extern int ref_level;

// void type function
extern int is_void;

void init();
void blk_push();
void blk_pop();
void all_program();
void all_func(struct Symtab *symbol);
void all_parm(struct Symtab *symbol);
void s_lookup(int yylex);
struct Symtab *s_find(const char *name);
struct Symtab *link_parm(struct Symtab *symbol, struct Symtab *next);
void all_var(struct Symtab *symbol, int size, int refLevel);
struct Symtab *make_func(struct Symtab *symbol, int returnQty);
void chk_parm(struct Symtab *symbol, int count);
int parm_default(struct Symtab *symbol);
int chk_var(struct Symtab *symbol);
void chk_func(struct Symtab *symbol);

