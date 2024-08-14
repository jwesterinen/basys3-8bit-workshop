/*
*
*   lexer.h
*
*   Basic interpreter lexer interface.
*
*/

// token values, single char tokens use their ASCII values
#define Numvar      300
#define Strvar      302
#define Function    333
#define Constant    304
#define String      305
#define PRINT       306
#define LET         307
#define FOR         308
#define TO          309
#define STEP        310
#define NEXT        311
#define IF          312
#define THEN        313
#define GOTO        314
#define GOSUB       315
#define RETURN      316
#define STOP        317
#define END         318
#define INPUT       319
#define AND_OP      320
#define NOT_OP      321
#define OR_OP       322
#define XOR_OP      323
#define NE_OP       324
#define GE_OP       325
#define LE_OP       326
#define MOD_OP      327
#define SL_OP       328
#define SR_OP       329
#define POKE        330
#define DIM         331

union LEXTYPE {
    Symbol *lexsym;  // the symbol created from a token
    char *lexeme;    // the literal text string of the token 
};
typedef union LEXTYPE LEXTYPE;
extern LEXTYPE lexval;
extern char tokenStr[80];
extern int token;

extern char gCommandStr[];
extern char *nextChar;

bool GetNextToken(char *commandStr);

