/*
*
*   lexer.h
*
*/

// token values, single char tokens use their ASCII values
#define Constant    300
#define IntvarName  301
#define StrvarName  302
#define String      303
#define PRINT       304
#define FOR         305
#define TO          306
#define STEP        307
#define NEXT        308
#define IF          309
#define THEN        310
#define GOTO        311
#define GOSUB       312
#define RETURN      313
#define END         314
#define AND_OP      315
#define NOT_OP      316
#define OR_OP       317
#define XOR_OP      318
#define NE_OP       319
#define GE_OP       320
#define LE_OP       321
#define MOD_OP      322
#define SL_OP       323
#define SR_OP       324
#define INPUT       325
#define LET         326

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

