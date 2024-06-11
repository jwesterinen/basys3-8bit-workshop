/*
*
*   lexer.h
*
*/

// token values, single char tokens use their ASCII values
#define Constant    300
#define Intvar      301
#define Strvar      302
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

extern char *nextChar;
extern int token;
extern char lexeme[];
extern Symbol *lexsym;

void GetNextToken(char *commandStr);

