/*
*
*   lexer.h
*
*/

extern int token;
extern char lexeme[80];
extern SymbolID lexsym;

void GetNextToken(char *commandStr);

