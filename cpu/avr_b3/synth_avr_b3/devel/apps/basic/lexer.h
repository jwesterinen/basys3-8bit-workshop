/*
*
*   lexer.h
*
*/

// token values, single char tokens use their ASCII values
#define Constant    300
#define Identifier  301
#define StrvarName  302
#define String      303
#define PRINT       304
#define LET         305
#define FOR         306
#define TO          307
#define STEP        308
#define NEXT        309
#define IF          310
#define THEN        311
#define GOTO        312
#define GOSUB       313
#define RETURN      314
#define STOP        315
#define END         316
#define INPUT       317
#define AND_OP      318
#define NOT_OP      319
#define OR_OP       320
#define XOR_OP      321
#define NE_OP       322
#define GE_OP       323
#define LE_OP       324
#define MOD_OP      325
#define SL_OP       326
#define SR_OP       327
#define POKE        328

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

