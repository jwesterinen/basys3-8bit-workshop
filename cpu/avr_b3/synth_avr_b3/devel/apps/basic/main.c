/*
*   This is the main module for the a Basic interpreter.
*/

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_lib.h"

/*
    <expr>          := <term> <expr-prime>
    <expr-prime>    := '+' <term> <expr-prime> | $
    <term>          := <factor> <term-prime>
    <term-prime>    := '*' <factor> <term-prime> | $
    <factor>        := '(' <expr> ')' | NUMBER
*/

void Console(void);

bool IsFactor(void), IsTermPrime(void), IsTerm(void), IsExprPrime(void), IsExpr(void), IsNumber(void);

char *instr;
char *token;
    

// <expr> := <term> <expr-prime>
bool IsExpr()
{
    stdout = &mystdout;
    
    //printf("%-16s E -> T E'\r\n", instr);
    if (IsTerm())
    {
        if (IsExprPrime())
        {
            return true;
        }
    }
    
    return false;
}

// <expr-prime> := '+' <term> <expr-prime> | $
bool IsExprPrime()
{
    stdout = &mystdout;
    
    if (!strcmp(token, "+"))
    {
        //printf("%-16s E' -> + T E'\r\n", instr);
        token = strtok(instr, NULL);
        if (IsTerm())
        {
            if (IsExprPrime())
            {
                return true;
            }
        }
    }
    else
    {
        //printf("%-16s E' -> $\r\n", instr);
        return true;
    }
    
    return false;
}

// <term> := <factor> <term-prime>
bool IsTerm()
{
    stdout = &mystdout;
    
    //printf("%-16s T -> F T'\r\n", instr);
    if (IsFactor())
    {
        if (IsTermPrime())
        {
            return true;
        }
    }
    
    return false;
}

// <term-prime> := '*' <factor> <term-prime> | $
bool IsTermPrime()
{
    stdout = &mystdout;
    
    if (!strcmp(token, "*"))
    {
        //printf("%-16s T' -> * F T'\r\n", instr);
        token = strtok(instr, NULL);
        if (IsFactor())
        {
            if (IsTermPrime())
            {
                return true;
            }
        }
    }
    else
    {
        return true;
    }
    
    return false;
}

// <factor> := '(' <expr> ')' | NUMBER
bool IsFactor()
{
    stdout = &mystdout;
    
    if (!strcmp(token, "("))
    {
        //printf("%-16s F -> ( E )\r\n", instr);
        token = strtok(instr, NULL);
        if (IsExpr())
        {
            if (!strcmp( token, ")"))
            {
                token = strtok(instr, NULL);
                return true;
            }
        }
    }
    else if (IsNumber())
    {
        //printf("%-16s F -> i\r\n", instr);
        return true;
    }
    
    return false;
}

bool IsNumber()
{
    //long value = 0;
    char *end;
    
    //if (!strcmp(token, "0") || ((value = strtol(token, &end, 10)) != 0))
    if (!strcmp(token, "0") || (strtol(token, &end, 10) != 0))
    {
        token = strtok(instr, NULL);
        return true;
    }
    
    return false;
}

// parse the instruction string
bool Parse(char* text)
{
    stdout = &mystdout;
    instr = text;
    
    //printf("\r\n");
    //printf("Input          Action\r\n");
    //printf("--------------------------------\r\n");
    printf("instruction: %s\r\n", text);
    
    token = strtok(instr, " \t");
    if (IsExpr() && *instr == '\0')
    {
        printf("...expression is valid\r\n");
        return true;
    }
    printf("...expression is invalid\r\n");
    
    return 0;
}

int main(void)
{
    // set UART baud rate to 115200
    UBRR0 = 13-1;

    Console();
}
