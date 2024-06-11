/*
*   parser.c
*
*   Basic interpreter parser.
*
*   Todo items:
*    - reclaim dynamic memory
*    - don't allocate nodes before checking syntax
*    - change all strings to dynamic allocation, i.e. Program, Strings
*    - syntax check for program lines before adding to Program
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
bool IsGoto(void);
bool ExecCommand(void);

bool ready = true;
char errorStr[STRING_LEN];
char resultStr[STRING_LEN];

typedef struct ForInstruction {
    int lineNum;
    Symbol *symbol;
    int to;
    int step;
} ForInstruction;
ForInstruction fortab[INSTR_TAB_LEN];
int fortabIdx = 0;

#define MAX_PROGRAM_LEN 100
typedef struct CodeLine {
    int lineNum;
    char command[STRING_LEN]; 
} CodeLine;
CodeLine Program[MAX_PROGRAM_LEN];
int programIdx = 0;     // program index used to add commands to the program
int ip = 0;             // instruction pointer used to point to the current command in the program

/*
    BASIC grammar:
    
    command         : directive | deferred-cmd | immediate-cmd
    directive       : RUN | LIST | NEW | BYE
    deferred-cmd    : [Constant] executable-cmd
    executable-cmd  : immediate-cmd | for | next | goto
    immediate-cmd   : print | assignment
    print           : PRINT print-list
    print-list      : printable  [';' | ','] print-list | printable
    printable       : expr | String
    assignment      : (Intvar '=' expr) | (Strvar '=' String)
    for             : FOR Identifier '=' expr TO expr [STEP expr]
    next            : NEXT [Identifier]
    
    goto            : GOTO Constant
    if              : IF expr THEN immediate-cmd | IF expr THEN [GOTO] Constant | IF expr [THEN] GOTO Constant
    gosub           :
    return          :
    
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

void SwapProgLines(int progIdxA, int progIdxB) 
{ 
    CodeLine temp;
    
    // copy a to temp
    temp.lineNum = Program[progIdxA].lineNum; 
    strcpy(temp.command, Program[progIdxA].command);
    
    // copy b to a
    Program[progIdxA].lineNum = Program[progIdxB].lineNum; 
    strcpy(Program[progIdxA].command, Program[progIdxB].command);
    
    // copy temp to b
    Program[progIdxB].lineNum = temp.lineNum; 
    strcpy(Program[progIdxB].command, temp.command);
} 

void SortProgramByLineNum(void) 
{ 
    int i, j, min_idx; 
  
    // One by one move boundary of unsorted subarray 
    for (i = 0; i < programIdx - 1; i++) 
    { 
        // Find the minimum element in unsorted array 
        min_idx = i; 
        for (j = i + 1; j < programIdx; j++) 
            if (Program[j].lineNum < Program[min_idx].lineNum) 
                min_idx = j; 
  
        // Swap the found minimum element with the first element 
        SwapProgLines(min_idx, i); 
    } 
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
    else if (!strcmp(command, "bye"))
    {
        exit(0);
    }
    
    // determine deferred or immediate command
    GetNextToken(command);
    if (token == Constant)
    {
        // deferred - add to program
        int lineNum = atoi(lexeme);
        int lineIdx;
        
        // check to see if this is a replacement of an existing line by number
        for (lineIdx = 0; lineIdx < programIdx; lineIdx++)
        {
            if (Program[lineIdx].lineNum == lineNum)
            {
                break;
            }
        }
        strcpy(Program[lineIdx].command, command);
        if (lineIdx == programIdx)
        {
            Program[lineIdx].lineNum = lineNum;
            if (programIdx < MAX_PROGRAM_LEN-1)
            {
                programIdx++;
            }
            else
            {
                sprintf(errorStr, "no more program space");
                return false;
            }
        }
        SortProgramByLineNum();
        ready = false;
    }
    else
    {
        // immediate - execute the command directly
        if (!ExecCommand())
            return false;
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

// command : print | assignment | for | next | if
bool IsCommand()
{
    if (token == Constant)
    {
        GetNextToken(NULL);
    }
    
    // print | assignment
    if (IsPrint() || IsAssign() || IsFor() || IsNext() || IsGoto())
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
    
// expr-list : (Identifier '$' | expr | String) [';' | ','] expr-list | (Identifier '$' | expr | String)
// create a result string to be printed
bool IsExprList()
{
    int exprVal;
    char exprStr[80];
    
    //puts("IsExprList");
    if (token == Strvar)
    {
        strcat(resultStr, SYM_STRVAL(lexsym));
        GetNextToken(NULL);
    }
    else if (GetExprValue(&exprVal))
    {
        // convert expr value to ascii and cat to existing result string
        sprintf(exprStr, "%d", exprVal);              
        strcat(resultStr, exprStr);
    }
    else if (token == String)
    {
        // cat the string to the result string
        strcat(resultStr, lexeme);
        GetNextToken(NULL);
    }
    else
    {
        return false;
    }
    
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
    
    return false;
}
    
// assignment : Intvar '=' expr | Strvar = String
bool IsAssign()
{
    if (token == Intvar)
    {
        //puts("IsAssign");
        GetNextToken(NULL);
        if (token == '=')
        {
            GetNextToken(NULL);
            if (GetExprValue(&(SYM_INTVAL(lexsym))))
            {
                SYM_TYPE(lexsym) = ST_INTVAR;
                return true;
            }
        }
    }
    else if (token == Strvar)
    {
        GetNextToken(NULL);
        if (token == '=')
        {
            GetNextToken(NULL);
            if (token == String)
            {
                // copy the string to the symbol's string value
                SYM_STRVAL(lexsym) = calloc(strlen(lexeme)+1, 1);
                if (SYM_STRVAL(lexsym))
                {
                    strcpy(SYM_STRVAL(lexsym), lexeme);
                    SYM_TYPE(lexsym) = ST_STRVAR;
                    return true;
                }
            }
        }
    }
    
    return false;
}

// for : FOR Intvar '=' expr TO expr STEP expr
bool IsFor()
{
    int to, step = 1;
    
    if (token == FOR)
    {
        //puts("IsFor");
        GetNextToken(NULL);
        if (token == Intvar)
        {
            GetNextToken(NULL);
            if (token == '=')
            {
                GetNextToken(NULL);
                if (GetExprValue(&(SYM_INTVAL(lexsym))))
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

// next : NEXT [Intvar]
bool IsNext()
{
    if (token == NEXT)
    {
        //puts("IsNext");
        // default associated for instruction to the top of the for instruction stack
        ForInstruction forInstr = fortab[fortabIdx - 1];
        
        GetNextToken(NULL);
        if (token == Intvar)
        {
            // find the explicit for instruction associated with the next instruction's var name
            for (int i = 0; i < fortabIdx; i++)
            {
                if (!strcmp(SYM_NAME(fortab[i].symbol), lexeme))
                {
                    forInstr = fortab[i];
                }
            }
        }
        
        // change the variable value based on the for instruction's step value
        SYM_INTVAL(forInstr.symbol) += forInstr.step;
        
        // check that the variable's value is in the range of the for instruction
        if (SYM_INTVAL(forInstr.symbol) <= forInstr.to)
        {
            // goto command at code line number following the for command
            ip = LineNum2Ip(forInstr.lineNum);
        }
        return true;
    }
    
    return false;
}

// goto : GOTO Constant
bool IsGoto()
{
    int destination;
    
    if (token == GOTO)
    {
        GetNextToken(NULL);
        if (GetExprValue(&destination))
        {
            if (destination > 0)
            {
                ip = LineNum2Ip(destination);
                return true;
            }
        }
    }        

    return false;
}


// end of runtime.c

