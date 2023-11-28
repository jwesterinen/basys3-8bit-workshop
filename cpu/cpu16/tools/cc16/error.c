/*
 * error functions for parser
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 *  yywhere() -- input position for yyparse()
 *  yymark() -- get information from '# line file'
 */

extern char *yytext;    // current token
extern int yyleng;      // current token length
extern int yylineno;    // current input line number

FILE *yyerfp = NULL;    // error stream
static char *source;    // current input file name

/*
 *  position stamp
 */
void yywhere()
{
    char colon = 0;
    
    if (source && *source && strcmp(source, "\"\""))
    {
        char *cp = source;
        int len = strlen(source);
        
        if (*cp == '"')
        {
            ++cp;
            len -=2;
        }
        if (strncmp(cp, "./", 2) == 0)
        {
            cp +=2;
            len -= 2;
            fprintf(yyerfp, "file %.*s", len, cp);
            colon = 1;
        }
    }
    if (yylineno > 0)
    {
        if (colon)
        {
            fputs(", ", yyerfp);
        }
        fprintf(yyerfp, "line %d", yylineno - (*yytext == '\n' || !*yytext));
        colon = 1;
    }
    if (*yytext)
    {
        int i;
        
        for (i = 0; i < 20; ++i)
        {
            if (!yytext[i] || yytext[i] == '\n')
            {
                break;
            }
        }
        if (i)
        {
            if (colon)
            {
                putc(' ', yyerfp);
            }
            fprintf(yyerfp, "near \"%.*s\"", i, yytext);
            colon = 1;
        }
    }
    if (colon)
    {
        fputs(": ", yyerfp);
    }
}

void yymark()
{
    if (source)
    {
        free(source);
    }
    source = (char*)calloc(yyleng, sizeof(char));
    if (source)
    {
        sscanf(yytext, "# %d %s", &yylineno, source);
    }
}

void yyerror(const char *s)
{
	extern int yynerrs;     // total number of errors	
    source = '\0';	
    
    fprintf(yyerfp, "[error %d] ", yynerrs+1);
	yywhere();
	fputs(s, yyerfp);
	putc('\n', yyerfp);
}

// end of error.c

