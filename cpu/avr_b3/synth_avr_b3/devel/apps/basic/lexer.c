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

*   "!="                        return NE_OP

*   {letter}{letter_or_digit}*$ return Strvar
*   {letter}{letter_or_digit}*  return Intvar
*   [-]{digit}+                 return Number
*   \"{.}*\"                    return String
*   {other}                     return .
*/

// TODO: make keywords case insensitive
struct KeywordTableEntry {
    char *keyword;
    int token;
} keywordTab[] = {
//       tokenStr   token
        {"print",   PRINT   },
        {"for",     FOR     },
        {"to",      TO      },
        {"step",    STEP    },
        {"next",    NEXT    },
        {"goto",    GOTO    },
        {"if",      IF      },
        {"then",    THEN    },
        {"gosub",   GOSUB   },
        {"return",  RETURN  },
        {"end",     END     },
        {"input",   INPUT   },
        {"let",     LET     },
        {"and",     AND_OP  },
        {"not",     NOT_OP  },
        {"or",      OR_OP   },
        {"xor",     XOR_OP  },
        {"mod",     MOD_OP  }
};
int keywordTableSize = sizeof keywordTab / sizeof(struct KeywordTableEntry);

char gCommandStr[STRING_LEN];
char *nextChar;
LEXTYPE lexval;
char tokenStr[STRING_LEN];
int token;

// return the next token in the instruction
bool GetNextToken(char *commandStr)
{
    static int state = 0;
    int i = 0;
    char tokenStrLc[80];
    
    // init the lexer on a new input string
    if (commandStr != NULL)
    {
        strcpy(gCommandStr, commandStr);
        nextChar = commandStr;
    }
    
    // TODO: clean up case values to be in sequence
    // lexer state machine
    while (1)
    {
        switch (state)
        {
            case 0:
                token = 0;
                // remove whitespace
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
                // direct the lexer to parse a number, variable name, string, or operator
                if (isdigit(*nextChar))
                {
                    tokenStr[i++] = *nextChar++;
                    state = 2;
                }
                else if (isalpha(*nextChar))
                {
                    tokenStr[i++] = *nextChar++;
                    state = 3;
                }
                else if (*nextChar == '"')
                {
                    nextChar++;
                    state = 10;
                }
                else if (*nextChar != '\0')
                {
                    tokenStr[0] = *nextChar++;
                    state = 6;
                }
                else
                {
                    state = 7;
                }
                break;
                
            case 2:
                // parse a number
                if (isdigit(*nextChar))
                {
                    tokenStr[i++] = *nextChar++;
                }
                else
                {
                    state = 4;
                }
                break;
                
            case 3:
                // parse variable name or keyword
                if (isalnum(*nextChar))
                {
                    tokenStr[i++] = *nextChar++;
                }
                else
                {
                    state = 5;
                }
                break;
                
            case 10:
                // parse a string ensuring the string is correctly terminated
               if (*nextChar != '"')
                {
                    if (*nextChar == '\0')
                    {
                        return false;
                    }
                    tokenStr[i++] = *nextChar++;
                }
                else
                {
                    nextChar++;
                    state = 11;
                }
                break;
                
            case 11:
                // return String token
                tokenStr[i] = '\0';
                token = String;
                if (SymLookup(token))
                {
                    i = 0;
                    state = 0;
                    return true;
                }
                return false;
                
            case 4:
                // return Constant token
                tokenStr[i] = '\0';
                token = Constant;
                if (SymLookup(token))
                {
                    i = 0;
                    state = 0;
                    return true;
                }
                return false;
                
            case 5:
                // check for string var name
                if (*nextChar == '$')
                {
                    tokenStr[i++] = *nextChar++;
                    token = StrvarName;
                }
                    
                // terminate the tokenStr and prepare for the next lexer state
                tokenStr[i] = '\0';
                i = 0;
                state = 0;
                
                // if the tokenStr is a keyword, normalize it to lower case and return its corresponding token
                for (int j = 0; j < strlen(tokenStr)+1; j++)
                {
                    tokenStrLc[j] = tolower(tokenStr[j]);
                }
                for (int j = 0; j < keywordTableSize; j++)
                {
                    if (!strcmp(keywordTab[j].keyword, tokenStrLc))
                    {
                        token = keywordTab[j].token;
                        return true;
                    }
                }
                
                // if the tokenStr isn't a keyword or a string var name return int var name
                if (token == 0)
                    token = IntvarName;
                    
                // set the symbol reference of the variable name
                return SymLookup(token);
                
            case 6:
                // return double- or single-char operator token
                if (tokenStr[0] == '!' && *nextChar == '=')
                {
                    tokenStr[i++] = *nextChar++;
                    token = NE_OP;
                }
                else if (tokenStr[0] == '<' && *nextChar == '>')
                {
                    tokenStr[i++] = *nextChar++;
                    token = NE_OP;
                }
                else if (tokenStr[0] == '<' && *nextChar == '=')
                {
                    tokenStr[i++] = *nextChar++;
                    token = LE_OP;
                }
                else if (tokenStr[0] == '>' && *nextChar == '=')
                {
                    tokenStr[i++] = *nextChar++;
                    token = GE_OP;
                }
                else if (tokenStr[0] == '<' && *nextChar == '<')
                {
                    tokenStr[i++] = *nextChar++;
                    token = SL_OP;
                }
                else if (tokenStr[0] == '>' && *nextChar == '>')
                {
                    tokenStr[i++] = *nextChar++;
                    token = SR_OP;
                }
                else
                {
                    token = tokenStr[0];
                }
                tokenStr[i] = '\0';
                i = 0;
                state = 0;
                return true;
                
            case 7:
                // return EOL
                i = 0;
                state = 0;
                token = 0;
                return true;
        }
    }
    
    return true;
}

// end of lexer.c

