/*
*   parser.c
*
*   Basic interpreter parser.
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
#define INSTR_TAB_LEN 100

extern void PrintResult(void);

bool RunProgram(void);
bool ListProgram(void);
bool IsCommand(void);
bool IsPrint(void);
bool IsExprList(void);
bool IsAssign(void);
bool IsFor(void);
bool IsNext(void);
bool ExecCommand(void);

bool ready = true;
char errorStr[STRING_LEN];
char resultStr[STRING_LEN];


typedef struct ForInstruction {
    int lineNum;
    SymbolID symbol;
    int to;
    int step;
} ForInstruction;
ForInstruction fortab[INSTR_TAB_LEN];
int fortabIdx = 0;

#define PROGRAM_LEN 100
struct CodeLine {
    int lineNum;
    char command[STRING_LEN]; 
} Program[PROGRAM_LEN];
int programIdx = 0;     // program index used to add commands to the program
int ip = 0;             // instruction pointer used to point to the current command in the program

/*
    BASIC grammar:
    
    command         : directive | deferred-cmd | immediate-cmd
    directive       : RUN | LIST | NEW
    deferred-cmd    : [Constant] executable-cmd
    executable-cmd  : immediate-cmd | if | for | next
    immediate-cmd   : print | assignment
    print           : PRINT print-list
    print-list      : printable  [';' | ','] print-list | printable
    printable       : expr | String
    assignment      : [LET] Identifier '=' [expr | String]
    if              : IF expr THEN immediate-cmd | IF expr THEN [GOTO] Constant | IF expr [THEN] GOTO Constant
    goto            : GOTO Constant
    for             : FOR Identifier '=' expr TO expr [STEP expr]
    next            : NEXT [Identifier]
    
    expr            : term expr-prime
    expr-prime      : ['+' | '-'] term expr-prime | $
    term            : factor term-prime
    term-prime      : ['*' | '/'] factor term-prime | $
    factor          : '(' expr ')' | Constant | Identifier
    string          : '"' (printable-char)+ '"'
*/

// convert a command line number to an index to the program
int LineNum2Ip(int lineNum)
{
    for (int i = 0; i < programIdx; i++)
        if (Program[i].lineNum == lineNum)
            return i;            
    return 0;
}

// convert an index to the program to a command line number
int Idx2lineNum(int idx)
{
    if (idx < programIdx)
        return Program[idx].lineNum;
    return 0;
}

// command : directive | statement
bool ProcessCommand(char *command)
{
    // init the parser and error sting
    InitParser();
    errorStr[0] = '\0';
    
    // execute any directive
    if (!strcmp(command, "run"))
    {
        return RunProgram();
    }
    else if (!strcmp(command, "list"))
    {
        return ListProgram();
    }
    else if (!strcmp(command, "new"))
    {
        return (programIdx = 0);
    }
    
    // determine deferred or immediate command
    GetNextToken(command);
    if (token == Constant)
    {
        // deferred - add to program
        strcpy(Program[programIdx].command, command);
        Program[programIdx].lineNum = atoi(lexeme);
        programIdx++;
        ready = false;
    }
    else
    {
        // execute the command directly
        ExecCommand();
        ready = true;
    }
    
    return true;
}

bool RunProgram(void)
{
    for (ip = 0; ip < programIdx; ip++)
    {
        // execute the command
        GetNextToken(Program[ip].command);
        if (!ExecCommand())
        {
            ip = 0;
            return false;
        }
    }
    ip = 0;
    ready = true;      
      
    return true;
}

bool ListProgram(void)
{
    for (int i = 0; i < programIdx; i++)
    {
        sprintf(resultStr, "%s", Program[i].command);
        PrintResult();
    }
    ready = true;
    
    return true;
}

bool ExecCommand()
{
    if (!IsCommand())
    {
        strcpy(errorStr, "syntax error");
        return false;
    }
    PrintResult();
    
    return true;
}

// command : print | assignment | if | for | next
bool IsCommand()
{
    if (token == Constant)
    {
        GetNextToken(NULL);
    }
    
    // print | assignment
#ifdef FOR_NEXT
    if (IsPrint() || IsAssign() || IsFor() || IsNext())
#else    
    if (IsPrint() || IsAssign())
#endif    
    {
        return true;
    }
    
    return false;
}

// print : PRINT expr-list
bool IsPrint()
{
    if (token == PRINT)
    {
        //puts("IsPrint");
        GetNextToken(NULL);
        if (IsExprList())
        {
            return true;
        }
    }
    
    return false;
}
    
// expr-list : (expr | String) [';' | ','] expr-list | (expr | String)
// create a result string to be printed
bool IsExprList()
{
    int exprVal;
    char exprStr[80];
    
    //puts("IsExprList");
    if (GetExprValue(&exprVal))
    {
        // convert expr value to ascii and cat to existing result string
        sprintf(exprStr, "%d", exprVal);              
        strcat(resultStr, exprStr);
        if (token == ';' || token == ',')
        {
            if (token == ',')
            {
                // cat intervening tab
                strcat(resultStr, "\t");                
            }
            GetNextToken(NULL);
            if (IsExprList())
            {
                return true;
            }
        }
        else
            return true;
    }
    
    return false;
}
    
// assignment : [let] Identifier '=' expr
bool IsAssign()
{
    if (token == LET)
    {
        // optional syntactic sugar
        GetNextToken(NULL);
    }
    if (token == Identifier)
    {
        //puts("IsAssign");
        GetNextToken(NULL);
        if (token == '=')
        {
            GetNextToken(NULL);
            if (GetExprValue(&(SYMVAL(lexsym))))
            {
                return true;
            }
        }
    }
    
    return false;
}

#ifdef FOR_NEXT
// for : FOR Identifier '=' expr TO expr STEP expr
bool IsFor()
{
    int to, step = 1;
    
    if (token == FOR)
    {
        //puts("IsFor");
        GetNextToken(NULL);
        if (token == Identifier)
        {
            GetNextToken(NULL);
            if (token == '=')
            {
                GetNextToken(NULL);
                if (GetExprValue(&(SYMVAL(lexsym))))
                {
                    //GetNextToken(NULL);
                    if (token == TO)
                    {
                        GetNextToken(NULL);
                        if (GetExprValue(&to))
                        {
                            //GetNextToken(NULL);
                            if (token == STEP)
                            {
                                if (!GetExprValue(&step))
                                {
                                    return false;
                                }
                            }
                            fortab[fortabIdx].lineNum = Program[ip].lineNum;
                            fortab[fortabIdx].symbol = lexsym;
                            fortab[fortabIdx].to = to;
                            fortab[fortabIdx].step = step;
                            fortabIdx++;
                            return true;
                        }
                    }
                }
            }
        } 
    }
    
    return false;       
}

// next : NEXT [Identifier]
bool IsNext()
{
    if (token == NEXT)
    {
        //puts("IsNext");
        // default associated for instruction to the top of the for instruction stack
        ForInstruction forInstr = fortab[fortabIdx - 1];
        
        GetNextToken(NULL);
        if (token == Identifier)
        {
            // find the explicit for instruction associated with the next instruction's var name
            for (int i = 0; i < fortabIdx; i++)
            {
                if (!strcmp(SYMNAME(fortab[i].symbol), lexeme))
                {
                    forInstr = fortab[i];
                }
            }
        }
        
        // change the variable value based on the for instruction's step value
        SYMVAL(forInstr.symbol) += forInstr.step;
        
        // check that the variable's value is in the range of the for instruction
        if (SYMVAL(forInstr.symbol) <= forInstr.to)
        {
            // goto command at code line number following the for command
            ip = LineNum2Ip(forInstr.lineNum);
        }
        return true;
    }
    
    return false;
}
#endif

// end of runtime.c

