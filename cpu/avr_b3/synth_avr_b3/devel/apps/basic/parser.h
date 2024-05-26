/*
*   parser.h
*
*/

// token values, single char tokens use their ASCII values
#define Constant    300
#define Identifier  301
#define PRINT       302
#define LET         303

extern char errorStr[];
extern char resultStr[];
    
bool ProcessCommand(char *exprStr);


