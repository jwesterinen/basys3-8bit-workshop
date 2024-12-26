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

typedef struct Command Command;

typedef struct Printable {
    PT_Node *expr;
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
    PT_Node *indexNodes[DIM_MAX];  // possible array index nodes
    PT_Node *expr;                 // RHS
} AssignCommand;

typedef struct ForCommand {
    int lineNum;
    Symbol *symbol;
    PT_Node *init;
    PT_Node *to;
    PT_Node *step;
} ForCommand;

typedef struct NextCommand {
    int lineNum;
    Symbol *symbol;
} NextCommand;

typedef struct GotoCommand {
    PT_Node *dest;
} GotoCommand;

enum IF_TYPE {IT_PRINT, IT_ASSIGN, IT_GOTO};
typedef struct IfCommand {
    PT_Node *expr;
    Command *commandList;
} IfCommand;

typedef struct GosubCommand {
    int lineNum;
    PT_Node *dest;
} GosubCommand;

typedef struct InputCommand {
    Symbol *varsym;                 // LHS symbol to which to assign an input value
    PT_Node *indexNodes[DIM_MAX];
} InputCommand;

typedef struct PlatformCommand {
    PT_Node *arg1;
    PT_Node *arg2;
    PT_Node *arg3;
} PlatformCommand;

typedef struct DimCommand {
    Symbol *varsym;                 // contains the linear data array
    PT_Node *dimSizeNodes[DIM_MAX];    // the expression of each dimension
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

