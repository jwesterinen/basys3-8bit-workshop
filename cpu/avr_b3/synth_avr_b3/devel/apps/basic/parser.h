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
    CT_PUTDB,
    CT_LOADFB,
    CT_CLEAR,
    CT_CLEARDB,
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

