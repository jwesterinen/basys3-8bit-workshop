/*
*   parser.h
*
*/

#define TABLE_LEN 100

// command data structures are loaded by the parser and used by the runtime

enum NodeType {
    NT_NONE = 0,
    NT_EXPR, 
    NT_LOGIC_EXPR, NT_LOGIC_EXPR_PRIME, 
    NT_REL_EXPR, NT_REL_EXPR_PRIME, 
    NT_SHIFT_EXPR, NT_SHIFT_EXPR_PRIME, 
    NT_ADD_EXPR, NT_ADD_EXPR_PRIME, 
    NT_MULT_EXPR, NT_MULT_EXPR_PRIME, 
    NT_UNARY_EXPR, 
    NT_POSTFIX_EXPR, NT_POSTFIX_EXPR_PRIME, NT_ARG_EXPR_LIST, 
    NT_PRIMARY_EXPR,
    NT_CONSTANT, NT_INTVAR, NT_STRVAR, NT_ARRAY, NT_FCT, NT_STRING, NT_BINOP, NT_UNOP
};

union NodeValue {
    int constant;
    Symbol *id;
    char *string;
    int op;
};
typedef struct Node {
    enum NodeType type;
    union NodeValue value;
    struct Node *bro;
    struct Node *son;
} Node;

#define NODE_TYPE(node)         (node)->type
#define NODE_VAL_CONST(node)    (node)->value.constant
#define NODE_VAL_ID(node)       (node)->value.id
#define NODE_VAL_STRING(node)   (node)->value.string
#define NODE_VAL_OP(node)       (node)->value.op
#define BRO(node)               (node)->bro
#define SON(node)               (node)->son

extern Node *ExprList[TABLE_LEN];
extern int exprListIdx;

// general value types
enum VALUE_TYPE {VT_STRSYM, VT_EXPR, VT_STRING, VT_FCT};

typedef struct Printable {
    char separator;
    enum VALUE_TYPE type;
    union {
        Symbol *symbol;
        Node *expr;
        char *string;
    } value;
} Printable;

typedef struct PrintCommand {
    Printable printList[20];
    int printListIdx;
} PrintCommand;

typedef struct AssignCommand {
    Symbol *varsym;             // LHS symbol
    enum VALUE_TYPE type;
    union {
        Node *expr;
        char *string;
    } value;                    // RHS value, either expr or string
} AssignCommand;

typedef struct ForCommand {
    int lineNum;
    Symbol *symbol;
    Node *init;
    Node *to;
    Node *step;
} ForCommand;

typedef struct NextCommand {
    Symbol *symbol;
} NextCommand;

typedef struct GotoCommand {
    Node *dest;
} GotoCommand;

enum IF_TYPE {IT_PRINT, IT_ASSIGN, IT_GOTO};
typedef struct IfCommand {
    Node *expr;
    enum IF_TYPE type;
    union {
        PrintCommand    printCmd;
        AssignCommand   assignCmd;
        GotoCommand     gotoCmd;
    } cmd;
} IfCommand;

typedef struct GosubCommand {
    int lineNum;
    Node *dest;
} GosubCommand;

typedef struct InputCommand {
    Symbol *varsym;
    enum VALUE_TYPE type;
    union {
        Node *expr;
        char string[80];
    } value; 
} InputCommand;

typedef struct PokeCommand {
    Node *addr;
    Node *data;
} PokeCommand;

enum COMMAND_TYPE {CT_PRINT, CT_ASSIGN, CT_FOR, CT_NEXT, CT_GOTO, CT_IF, CT_GOSUB, CT_RETURN, CT_END, CT_INPUT, CT_POKE};
typedef struct Command {
    char commandStr[80];
    int lineNum;
    enum COMMAND_TYPE type;
    union {
        PrintCommand    printCmd;
        AssignCommand   assignCmd;
        ForCommand      forCmd;
        NextCommand     nextCmd;
        GotoCommand     gotoCmd;
        IfCommand       ifCmd;
        GosubCommand    gosubCmd;
        InputCommand    inputCmd;
        PokeCommand     pokeCmd;
    } cmd;
} Command;

bool IsCommand(Command *command, bool *isImmediate);
void FreeExprTrees(void);

