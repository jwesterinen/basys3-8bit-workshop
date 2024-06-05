/*
*
*   lexer.h
*
*/

// token values, single char tokens use their ASCII values
#define Constant    300
#define Identifier  301
#define String      302
#define PRINT       303
#define LET         304
#define IF          305
#define THEN        306
#define GOTO        307
#define FOR         308
#define TO          309
#define STEP        310
#define NEXT        311

extern char *nextChar;
extern int token;
extern char lexeme[];
extern SymbolID lexsym;

void GetNextToken(char *commandStr);

