/*
*   parser.c
*
*   Basic interpreter parser.
*/

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"

void GetNextToken(void);
bool IsFactor(void), IsTermPrime(void), IsTerm(void), IsExprPrime(void), IsExpr(void), IsNumber(void);

// token values, single char tokens use their ASCII values
#define NUMBER  300
#define ID      301

char *nextChar;
int token;
char lexeme[20];
int i;
char errorStr[80];

/*
*   return the next token in the command buffer following these rules:
*
*   letter          [a-zA-Z_]
*   digit           [0-9]
*   letter_or_digit [a-zA-Z_0-9]
*   
*   {letter}{letter_or_digit}*  return Identifier
*   {digit}+                    return Number
*   {other}                     return .
*/
void GetNextToken()
{
    static int state = 0;
    
    //printf("\tGetNextToken: \n");    
    while (1)
    {
        switch (state)
        {
            case 0:
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
                // direct the lexer to parse a number, ID, or single char
                //printf("\t\tstate 1, direct lexer\n");
                if (isdigit(*nextChar))
                {
                    lexeme[i++] = *nextChar++;
                    state = 2;
                }
                else if (isalpha(*nextChar))
                {
                    lexeme[i++] = *nextChar++;
                    state = 3;
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
                // parse an ID
                //printf("\t\tstate 4, parse in ID\n");
                if (isalnum(*nextChar))
                {
                    lexeme[i++] = *nextChar++;
                }
                else
                {
                    state = 5;
                }
                break;
                
            case 4:
                // return NUMBER token
                lexeme[i] = '\0';
                //printf("\t\tstate 5, token = NUMBER %s\n", lexeme);
                i = 0;
                state = 0;
                token = NUMBER;
                return;
                
            case 5:
                // return ID token
                lexeme[i] = '\0';
                //printf("\t\tstate 6, token = ID %s\n", lexeme);
                i = 0;
                state = 0;
                token = ID;
                return;
                
            case 6:
                // return single char token
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

/*
    expression grammar:
    
    <expr>          := <term> <expr-prime>
    <expr-prime>    := '+' <term> <expr-prime> | $
    <term>          := <factor> <term-prime>
    <term-prime>    := '*' <factor> <term-prime> | $
    <factor>        := '(' <expr> ')' | NUMBER | ID
*/

// <expr> := <term> <expr-prime>
bool IsExpr()
{
    stdout = &mystdout;
    //printf("%d E -> T E'\n\r", token);
    if (IsTerm())
    {
        if (IsExprPrime())
            return true;
        else
            return false;
    }
    
    return false;
}

// <expr-prime> := '+' <term> <expr-prime> | $
bool IsExprPrime()
{
    stdout = &mystdout;
    if (token == '+')
    {
        //printf("%d E' -> + T E'\n\r", token);
        GetNextToken();
        if (IsTerm())
        {
            if (IsExprPrime())
                return true;
            else
                return false;
        }
    }

    //printf("%d E' -> $\n\r", token);
    return true;
}

// <term> := <factor> <term-prime>
bool IsTerm()
{
    stdout = &mystdout;
    //printf("%d T -> F T'\n\r", token);
    if (IsFactor())
    {
        if (IsTermPrime())
            return true;
        else
            return false;
    }

    return false;
}

// <term-prime> := '*' <factor> <term-prime> | $
bool IsTermPrime()
{
    stdout = &mystdout;
    if (token == '*')
    {
        //printf("%d T' -> * F T'\n\r", token);
        GetNextToken();
        if (IsFactor())
        {
            if (IsTermPrime())
                return true;
            else
                return false;
        }
    }

    //printf("%d T' -> $\n\r", token);
    return true;
}

// <factor> := '(' <expr> ')' | NUMBER | ID
bool IsFactor()
{
    stdout = &mystdout;
    if (token == '(')
    {
        //printf("%d F -> ( E )\n\r", token);
        GetNextToken();
        if (IsExpr())
        {
            if (token == ')')
            {
                GetNextToken();
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else if (token == NUMBER)
    {
        //printf("%d F -> NUMBER\n\r", token);
        GetNextToken();
        return true;
    }
    else if (token == ID)
    {
        //printf("%d F -> ID\n\r", token);
        GetNextToken();
        return true;
    }
    
    return false;
}

bool Parse(char *exprStr)
{
    nextChar = exprStr;
    GetNextToken();
    if (!IsExpr())
    {
        strcpy(errorStr, "syntax error");
        return false;
    }
    
    return true;
}


// end of parser.c

