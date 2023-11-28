/*
 *  gen_direct.h - code generator definitions
 */

// pseudo op codes
#define OP_BEGIN    "begin"     // 
#define OP_ALU      "alu"       // arithmetic/logic op
#define OP_DEC      "dec"       // region, offset
#define OP_POST_DEC "pdec"      // region, offset
#define OP_INC      "inc"       // region, offset
#define OP_POST_INC "pinc"      // region, offset
#define OP_ADDR     "addr"      // address of op
#define OP_INDIR    "indir"     // indirection op, i.e. contents of op
#define OP_LOAD     "load"      // region, offset
#define OP_STORE    "store"     // region, offset
#define OP_POP_RET  "popret"    // pop return value
#define OP_POP_ARG  "poparg"    // discard TOS
#define OP_JUMPZ    "jumpz"     // label
#define OP_JUMP     "jump"      // label
#define OP_CALL     "call"      // parm-count, address
#define OP_ENTRY    "entry"     // local-frame-size
#define OP_RETURN   "return"    // return from function
#define OP_END      "end"       // 

// region modifiers
#define MOD_GLOBAL  "gbl"       // global region
#define MOD_PARAM   "par"       // parameter region
#define MOD_LOCAL   "lcl"       // local region
#define MOD_IMMED   "con"       // constant (load only)
#define MOD_FCT     "fct"       // constant (load only)

// OP_ALU binary op modifiers
#define ALU_ADD     "+"         // addition
#define ALU_SUB     "-"         // subtract
#define ALU_MUL     "*"         // multiplication
#define ALU_DIV     "/"         // division
#define ALU_MOD     "%"         // remainder
#define ALU_LT      "<"         // compare as: <
#define ALU_GT      ">"         //             >
#define ALU_LE      "<="        //             <=
#define ALU_GE      ">="        //             >=
#define ALU_EQ      "=="        //             ==
#define ALU_NE      "!="        //             !=
#define ALU_AND     "&"         // bitwise and
#define ALU_OR      "|"         // bitwise or
#define ALU_XOR     "^"         // bitwise xor  

// OP_ALU unary op modifiers
#define ALU_INV     "~"         // bitwise inversion
#define ALU_NEG     "neg"       // negate (-)
#define ALU_NOT     "!"         // logical inversion (!)

// OP_ALU logical op modifiers
#define ALU_LAND    "&&"        // logical AND
#define ALU_LOR     "||"        // logical OR

void gen_begin_prog();
void gen_end_prog();
void gen_alu(const char *mod, const char *comment);
void gen_load_immed(const char *constant);
char *gen_mod(struct Symtab *symbol);
void gen_direct(const char *op, const char *mod, int val, const char *comment);
void gen_indirect(const char *op, const char *mod, int val, const char *comment, int is_rhs);
void gen_pointer(const char *op, const char *mod, int val, const char *comment, int is_rhs);
void gen_reference(const char *op, const char *mod, int val, const char *comment);
void gen_pop(const char *op, const char *comment);
void gen_return(const char *op, const char *comment);
int gen_jump(const char *op, int label, const char *comment);
int new_label();
int gen_label(int label);
void push_break(int label);
void push_continue(int label);
void pop_break();
void pop_continue();
void gen_break();
void gen_continue();
void gen_call(struct Symtab *symbol, int count);
int gen_entry(struct Symtab *symbol);
void fix_entry(struct Symtab *symbol, int label);
void end_program();

