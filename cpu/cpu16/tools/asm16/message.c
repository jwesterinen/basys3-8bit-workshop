/*
 *  message.c - message routines
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "error.h"

extern FILE *yyerfp;

#define PRINT_MSG                   \
    va_list args;                   \
    va_start(args, fmt);            \
    yywhere();                      \
    vfprintf(yyerfp, fmt, args);    \
    putc('\n', yyerfp);             \
    va_end(args)

void message(const char* fmt, ...)
{
    PRINT_MSG;
}

void error(const char* fmt, ...) 
{
    extern int yynerrs;
    fprintf(yyerfp, "[error %d] ", yynerrs++);
    PRINT_MSG;
}

void warning(const char* fmt, ...) 
{
    fprintf(yyerfp, "[warning] ");
    PRINT_MSG;
}

void fatal(const char* fmt, ...) 
{
    fprintf(yyerfp, "[fatal error] ");
    PRINT_MSG;
    exit(1);
}

void bug(const char* fmt, ...) 
{
    fprintf(yyerfp, "[BUG] ");
    PRINT_MSG;
    exit(1);
}

