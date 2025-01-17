/*
*
*   lexer.h
*
*   Basic interpreter lexer interface.
*
*/

// token values, single char tokens use their ASCII values
#define EOL         0
#define Numvar      300
#define Strvar      301
#define Function    302
#define Constant    303
#define String      304
#define PRINT       305
#define PRINTX      306
#define PRINTA      307
#define LET         308
#define FOR         309
#define TO          310
#define STEP        311
#define NEXT        312
#define IF          313
#define THEN        314
#define GOTO        315
#define GOSUB       316
#define RETURN      317
#define STOP        318
#define END         319
#define INPUT       320
#define AND_OP      321
#define NOT_OP      322
#define OR_OP       323
#define XOR_OP      324
#define NE_OP       325
#define GE_OP       326
#define LE_OP       327
#define MOD_OP      328
#define SL_OP       329
#define SR_OP       330
#define POKE        331
#define DIM         332
#define BREAK       333
#define TONE        334
#define BEEP        335
#define LEDS        336
#define DISPLAY     337
#define PUTCHAR     338
#define PUTDB       339
#define LOADFB      340
#define CLEAR       341
#define CLEARDB     342
#define OUTCHAR     343
#define RSEED       344
#define DELAY       345
#define TEXT        346
#define GR          347

union LEXTYPE {
    Symbol *lexsym;  // the symbol created from a token
    char *lexeme;    // the literal text string of the token 
};
typedef union LEXTYPE LEXTYPE;
extern LEXTYPE lexval;
extern char tokenStr[80];
extern int token;

//extern char gCommandStr[];
extern char *nextChar;

struct BuiltinFctTableEntry {
    char *name;
    int arity;
};
extern struct BuiltinFctTableEntry builtinFctTab[];
extern int builtinFctTableSize;

bool GetNextToken(char *commandStr);

