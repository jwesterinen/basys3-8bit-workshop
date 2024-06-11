/*
*   lexer.c
*
*   Basic interpreter lexical analyzer.
*/

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h>
#include "symtab.h"
#include "lexer.h"
#include "parser.h"

#define STRING_LEN 80

/*
*   lexical grammar
*
*   letter          [a-zA-Z_]
*   digit           [0-9]
*   letter_or_digit [a-zA-Z_0-9]
*   
*   "print"                     return PRINT
*   "let"                       return LET
*   {letter}{letter_or_digit}*$ return Strvar
*   {letter}{letter_or_digit}*  return Intvar
*   [-]{digit}+                    return Number
*   \"{.}*\"                    return String
*   {other}                     return .
*/

struct KeywordTableEntry {
    char *keyword;
    int token;
} keywordTab[] = {
//       lexeme     token
        {"print",   PRINT   },
        {"for",     FOR     },
        {"to",      TO      },
        {"step",    STEP    },
        {"next",    NEXT    },
        
        {"goto",    GOTO    },
        {"if",      IF      },
        {"then",    THEN    },
        {"gosub",   GOSUB   },
        {"return",  RETURN  }
};
int keywordTableSize = sizeof keywordTab / sizeof(struct KeywordTableEntry);

char *nextChar;
int token;
char lexeme[STRING_LEN];
Symbol *lexsym;

// return the next token in the instruction
void GetNextToken(char *codeStr)
{
    static int state = 0;
    int i = 0;
    
    //printf("\tGetNextToken: \n");
    
    // init the lexer on a new input string
    if (codeStr != NULL)
    {
        nextChar = codeStr;
    }
    
    // lexer state machine
    while (1)
    {
        switch (state)
        {
            case 0:
                token = 0;
                // remove whitespace
                //printf("\t\tstate 0, remove whitespace\n");
                if (isspace(*nextChar))
                {
                    nextChar++;
                }
                else
                {
                    state = 1;
                }
                break;
                
            case 1:
                // direct the lexer to parse a number, variable name, string, or single char
                //printf("\t\tstate 1, direct lexer\n");
                if (*nextChar == '-' || isdigit(*nextChar))
                {
                    lexeme[i++] = *nextChar++;
                    state = 2;
                }
                else if (isalpha(*nextChar))
                {
                    lexeme[i++] = *nextChar++;
                    state = 3;
                }
                else if (*nextChar == '"')
                {
                    nextChar++;
                    state = 10;
                }
                else if (*nextChar != '\0')
                {
                    lexeme[0] = *nextChar++;
                    state = 6;
                }
                else
                {
                    state = 7;
                }
                break;
                
            case 2:
                // parse a number
                //printf("\t\tstate 3, parse a number\n");
                if (isdigit(*nextChar))
                {
                    lexeme[i++] = *nextChar++;
                }
                else
                {
                    state = 4;
                }
                break;
                
            case 3:
                // parse variable name or keyword
                //printf("\t\tstate 4, parse in variable name or keyword\n");
                if (isalnum(*nextChar))
                {
                    lexeme[i++] = *nextChar++;
                }
                else
                {
                    state = 5;
                }
                break;
                
            case 10:
                // parse a string
                //printf("\t\tstate 10, parse string\n");
                if (*nextChar != '"')
                {
                    lexeme[i++] = *nextChar++;
                }
                else
                {
                    nextChar++;
                    state = 11;
                }
                break;
                
            case 11:
                // return String token
                lexeme[i] = '\0';
                //printf("\t\tstate 11, token = String %s\n", lexeme);
                i = 0;
                state = 0;
                token = String;
                return;
                
            case 4:
                // return Constant token
                lexeme[i] = '\0';
                //printf("\t\tstate 5, token = Constant %s\n", lexeme);
                i = 0;
                state = 0;
                token = Constant;
                return;
                
            case 5:
                // check for string var name
                if (*nextChar == '$')
                {
                    lexeme[i++] = *nextChar++;
                    token = Strvar;
                }
                    
                // terminate the lexeme and prepare for the next lexer state
                lexeme[i] = '\0';
                i = 0;
                state = 0;
                
                // if the lexeme is a keyword, return its corresponding token
                for (int j = 0; j < keywordTableSize; j++)
                {
                    if (!strcmp(keywordTab[j].keyword, lexeme))
                    {
                        token = keywordTab[j].token;
                        return;
                    }
                }
                
                // if the lexeme isn't a keyword or a string var name return Intvar
                if (token == 0)
                    token = Intvar;
                    
                // set the symbol reference of the variable name
                lexsym = SymLookup(lexeme);
                return;
                
            case 6:
                // return single char token
                lexeme[1] = '\0';
                //printf("\t\tstate 7, token = single char token %c\n", lexeme[0]);
                i = 0;
                state = 0;
                token = lexeme[0];
                return;
                
            case 7:
                // return EOL
                //printf("\t\tstate 8, token = EOL\n");
                i = 0;
                state = 0;
                token = 0;
                return;
        }
    }
}

// end of lexer.c

