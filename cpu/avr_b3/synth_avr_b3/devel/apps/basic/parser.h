/*
*   parser.h
*
*/

#define STRING_LEN 80
#define TABLE_LEN 100
#define MAX_PROGRAM_LEN 100
#define MAX_CMDLINE_LEN 80

extern char errorStr[];

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
    NT_POSTFIX_EXPR, NT_POSTFIX_EXPR_PRIME, NT_SUB_EXPR_LIST, 
    NT_PRIMARY_EXPR,
    
    // operator types
    NT_BINOP, NT_UNOP,
    
    // primary expression types
    NT_CONSTANT, NT_NUMVAR, NT_STRVAR, NT_FCT, NT_STRING
};

union NodeValue {
    Symbol *varsym;
    float constant;
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
#define NODE_VAL_VARSYM(node)   (node)->value.varsym
#define NODE_VAL_CONST(node)    (node)->value.constant
#define NODE_VAL_STRING(node)   (node)->value.string
#define NODE_VAL_OP(node)       (node)->value.op
#define BRO(node)               (node)->bro
#define SON(node)               (node)->son

extern Node *ExprList[TABLE_LEN];
extern int exprListIdx;

typedef struct Command Command;

typedef struct Printable {
    Node *expr;
    char separator;
} Printable;
enum PrintStyle {PS_DECIMAL, PS_HEX, PS_ASCII};
typedef struct PrintCommand {
    Printable printList[20];
    int printListIdx;
    enum PrintStyle style;
} PrintCommand;

typedef struct AssignCommand {
    Symbol *varsym;             // LHS symbol to which to assign a RHS value
    Node *indexNodes[DIM_MAX];  // possible array index nodes
    Node *expr;                 // RHS
} AssignCommand;

typedef struct ForCommand {
    int lineNum;
    Symbol *symbol;
    Node *init;
    Node *to;
    Node *step;
} ForCommand;

typedef struct NextCommand {
    int lineNum;
    Symbol *symbol;
} NextCommand;

typedef struct GotoCommand {
    Node *dest;
} GotoCommand;

enum IF_TYPE {IT_PRINT, IT_ASSIGN, IT_GOTO};
typedef struct IfCommand {
    Node *expr;
    Command *commandList;
} IfCommand;

typedef struct GosubCommand {
    int lineNum;
    Node *dest;
} GosubCommand;

typedef struct InputCommand {
    Symbol *varsym;                 // LHS symbol to which to assign an input value
    Node *indexNodes[DIM_MAX];
} InputCommand;

typedef struct PlatformCommand {
    Node *arg1;
    Node *arg2;
    Node *arg3;
} PlatformCommand;

typedef struct DimCommand {
    Symbol *varsym;                 // contains the linear data array
    Node *dimSizeNodes[DIM_MAX];    // the expression of each dimension
} DimCommand;

enum EX_COMMAND_TYPE {
    CT_NOP,
    CT_PRINT,
    CT_ASSIGN,
    CT_FOR,
    CT_NEXT,
    CT_GOTO,
    CT_IF,
    CT_GOSUB,
    CT_RETURN,
    CT_END,
    CT_INPUT,
    CT_POKE,
    CT_DIM,
    CT_BREAK,
    CT_TONE,
    CT_BEEP,
    CT_LEDS,
    CT_DISPLAY,
    CT_PUTCHAR,
    CT_CLEAR,
    CT_TEXT,
    CT_GR,
    CT_OUTCHAR,
    CT_RSEED,
    CT_DELAY
};

typedef struct Command {
    int lineNum;
    enum EX_COMMAND_TYPE type;
    union {
        PrintCommand    printCmd;
        AssignCommand   assignCmd;
        ForCommand      forCmd;
        NextCommand     nextCmd;
        GotoCommand     gotoCmd;
        IfCommand       ifCmd;
        GosubCommand    gosubCmd;
        InputCommand    inputCmd;
        PlatformCommand platformCmd;
        DimCommand      dimCmd;
    } cmd;
    struct Command *next;
} Command;

typedef struct CommandLine {
    char commandStr[MAX_CMDLINE_LEN];
    int lineNum;
    Command *commandList;
} CommandLine;    

extern CommandLine Program[MAX_PROGRAM_LEN];
extern int programSize;
    
bool IsCommandList(Command **ppCommandList, int lineNum);
bool IsExpr(Node **ppNode);
void FreeExprTrees(void);

