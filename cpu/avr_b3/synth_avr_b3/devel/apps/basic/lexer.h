/*
*
*   lexer.h
*
*   Basic interpreter lexer interface.
*
*/

// token values, single char tokens use their ASCII values
#define Intvar      300
#define Strvar      301
#define Function    332
#define Constant    303
#define String      304
#define PRINT       305
#define LET         306
#define FOR         307
#define TO          308
#define STEP        309
#define NEXT        310
#define IF          311
#define THEN        312
#define GOTO        313
#define GOSUB       314
#define RETURN      315
#define STOP        316
#define END         317
#define INPUT       318
#define AND_OP      319
#define NOT_OP      320
#define OR_OP       321
#define XOR_OP      322
#define NE_OP       323
#define GE_OP       324
#define LE_OP       325
#define MOD_OP      326
#define SL_OP       327
#define SR_OP       328
#define POKE        329
#define DIM         330

union LEXTYPE {
    Symbol *lexsym;  // Identifier
    char *lexeme;    // Constant or String 
};
typedef union LEXTYPE LEXTYPE;
extern LEXTYPE lexval;
extern char tokenStr[80];
extern int token;

extern char gCommandStr[];
extern char *nextChar;

bool GetNextToken(char *commandStr);

