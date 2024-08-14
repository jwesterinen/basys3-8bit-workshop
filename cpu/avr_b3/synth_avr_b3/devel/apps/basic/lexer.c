/*
*   lexer.c
*
*   A Basic grammar lexical analyzer for a Basic language interpreter.
*
*   This module scans the characters typed into the user interface of 
*   the interpreter and returns token types, lexemes, and symbol references.
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
*   Basic language lexical grammar
*
*   letter          [a-zA-Z_]
*   digit           [0-9]
*   letter_or_digit [a-zA-Z_0-9]
*   hextet          [a-fA-F0-9]
*   hex_prefix      0[xX]
*   other           return .
*   
*   {letter}{letter_or_digit}*  return Numvar
*   {letter}{letter_or_digit}*$ return Strvar
*   {digit}+[.{digit}+]         return Constant
*   {hex_prefix}{hextet}+       return HexConstant;
*   "{.}*"                      return String
*
*   "print"                     return PRINT
*   "let"                       return LET
*   "for"                       return FOR
*   "to"                        return TO
*   "step"                      return STEP
*   "next"                      return NEXT
*   "goto"                      return GOTO
*   "if"                        return IF
*   "then"                      return THEN
*   "gosub"                     return GOSUB
*   "return"                    return RETURN
*   "stop"                      return STOP
*   "end"                       return END
*   "input"                     return INPUT
*   "poke"                      return POKE
*   "dim"                       return DIM
*
*   "!="                        return NE_OP
*   "<>"                        return NE_OP
*   "<="                        return LE_OP
*   ">="                        return GE_OP
*   "<<"                        return SL_OP
*   ">>"                        return SR_OP
*
*/

struct KeywordTableEntry {
    char *keyword;
    int token;
} keywordTab[] = {
//       tokenStr   token
        {"print",   PRINT   },
        {"let",     LET     },
        {"for",     FOR     },
        {"to",      TO      },
        {"step",    STEP    },
        {"next",    NEXT    },
        {"goto",    GOTO    },
        {"if",      IF      },
        {"then",    THEN    },
        {"gosub",   GOSUB   },
        {"return",  RETURN  },
        {"stop",    STOP    },
        {"end",     END     },
        {"input",   INPUT   },
        {"poke",    POKE    },
        {"dim",     DIM     },
        {"and",     AND_OP  },
        {"not",     NOT_OP  },
        {"or",      OR_OP   },
        {"xor",     XOR_OP  },
        {"mod",     MOD_OP  }
};
int keywordTableSize = sizeof keywordTab / sizeof(struct KeywordTableEntry);

char *builtinFctTab[] = {
    "peek",
};
int builtinFctTableSize = sizeof builtinFctTab / sizeof(char *);

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
                // direct the lexer to scan an Identifier, Constant, String, or operator
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
                // attemtp to scan a hex number
                state = 333;
                if ((tokenStr[0] == '0') && (*nextChar == 'x' || *nextChar == 'X'))
                {
                    tokenStr[i++] = *nextChar++;
                    state = 222;
                }
                break;
                
            case 222:
                // continue to scan a hex number
                if (isdigit(*nextChar) || isxdigit(*nextChar))
                {
                    tokenStr[i++] = *nextChar++;
                }
                else
                {
                    state = 4;
                }
                break;

            case 333:
                // scan a decimal number
                if (isdigit(*nextChar))
                {
                    tokenStr[i++] = *nextChar++;
                }
                else if (*nextChar == '.')
                {
                    tokenStr[i++] = *nextChar++;
                    state = 20;
                }
                else
                {
                    state = 4;
                }
                break;
                
            case 20:
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
                // scan variable name or keyword
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
                // scan a string ensuring the string is correctly terminated
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
                    token = Strvar;
                }
                    
                // terminate the tokenStr and prepare for the next lexer state
                tokenStr[i] = '\0';
                i = 0;
                state = 0;
                
                // normalize the token string to lower case
                for (int j = 0; j < strlen(tokenStr)+1; j++)
                {
                    tokenStrLc[j] = tolower(tokenStr[j]);
                }
                
                // if the token string is a keyword return its corresponding token
                for (int j = 0; j < keywordTableSize; j++)
                {
                    if (!strcmp(keywordTab[j].keyword, tokenStrLc))
                    {
                        token = keywordTab[j].token;
                        return true;
                    }
                }
                
                // if the token string is a builtin fct id set the token type to Function
                for (int j = 0; j < builtinFctTableSize; j++)
                {
                    if (!strcmp(builtinFctTab[j], tokenStrLc))
                    {
                        token = Function;
                    }
                }
                
                // if the tokenStr isn't a keyword or a string var name, return Numvar (floating pt. variable)
                if (token == 0)
                {
                    token = Numvar;
                }
                    
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

