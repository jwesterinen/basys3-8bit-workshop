/*
*   parser.c
*
*   A Basic grammar parser for a Basic language interpreter.
*
*   This module will parse a Basic language command into its internal 
*   representation for later execution.
*
    BASIC grammar:
    
    command-line
        : directive
        | [Constant] command-list
        | 
        ;
    directive
        : RUN
        | LIST
        | NEW
        | REBOOT
        | TEXT
        | GR
        | MOUNT
        | UNMOUNT
        | LIST
        | DELETE filename
        | LOAD filename
        | SAVE filename
        ;
    command-list
        : command [':' command-list]
        ;
    command  
        : for
        | next
        | goto
        | gosub
        | return
        | stop
        | end
        | delay
        | print
        | assign
        | if
        | input
        | poke
        | tone
        | beep
        | leds
        | display
        | putchar
        | clear
        | outchar
        | rseed
        | dim
        | text
        | gr
        ;
    print
        : {PRINT | '?'} expr-list
        : PRINTX expr
        : PRINTA expr
        ;
    expr-list
        : expr  [{';' | ','} expr-list]
        ;
    assign
        : [let] Numvar ['(' expr [',' expr]* ')'] '=' expr 
        | [let] Strvar ['(' expr [',' expr]* ')'] '=' {String | postfixExpr}
        ;
    for
        : FOR Numvar '=' expr TO expr [STEP expr]
        ;
    next
        : NEXT [Numvar]
        ;
    goto
        : GOTO Constant
        ;
    if
        : IF expr THEN command-list
        ;
    gosub   
        : GOSUB Constant
        ;
    return
        : RETURN
        ;
    stop
        : END
        ;
    end
        : END
        ;
    input
        : INPUT {Numvar | Strvar} ['(' expr [',' expr]* ')']
        ;
    poke    
        : POKE expr ',' expr
        ;
    tone
        : TONE expr [',' expr]
        ;
    beep
        : BEEP
        ;
    leds
        : LEDS expr
        ;
    display
        : DISPLAY expr ',' expr
        ;
    text
        : TEXT
        ;
    gr
        : GR
        ;
    putchar
        : PUTCHAR expr ',' expr ',' expr
        ;
    clear
        : CLEAR
        ;
    outchar
        : OUTCHAR expr
        ;
    rseed
        : RSEED expr
        ;
    delay
        : DELAY expr
        ;
    dim
        : DIM {Numvar | Strvar} '(' expr [',' expr]+ ')'
        ;

*/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "symtab.h"
#include "lexer.h"
#include "ir.h"
#include "parser.h"
#include "main.h"

char *Type2Name(enum NodeType type);
bool IsExecutableCommand(Command *pCommand);
bool IsNop(Command *pCommand);
bool IsPrint(Command *pCommand);
bool IsExprList(PrintCommand *cmd, int *listIdx);
bool IsAssign(Command *pCommand);
bool IsFor(Command *pCommand);
bool IsNext(Command *pCommand);
bool IsGoto(Command *pCommand);
bool IsIf(Command *pCommand);
bool IsGosub(Command *pCommand);
bool IsReturn(Command *pCommand);
bool IsEnd(Command *pCommand);
bool IsInput(Command *pCommand);
bool IsPoke(Command *pCommand);
bool IsTone(Command *pCommand);
bool IsBeep(Command *pCommand);
bool IsLeds(Command *pCommand);
bool IsDisplay(Command *pCommand);
bool IsPutchar(Command *pCommand);
bool IsClear(Command *pCommand);
bool IsText(Command *pCommand);
bool IsGr(Command *pCommand);
bool IsOutchar(Command *pCommand);
bool IsRseed(Command *pCommand);
bool IsDelay(Command *pCommand);
bool IsDim(Command *pCommand);
bool IsBreak(Command *pCommand);

char errorStr[STRING_LEN];

// parse a possible list of colon-delineated executable commands on one line
// command : [Constant] command-list
bool IsCommandList(Command **ppCommandList, int lineNum)
{
    Command *pCommand, *next;
    
    // create a possible list of commands
    *ppCommandList = pCommand = (Command *)calloc(1, sizeof(Command));
    if (*ppCommandList == NULL)
    {
        Panic("system error: memory allocation error while parsing command line\n");
        return false;
    }
    pCommand->lineNum = lineNum;
    if (IsExecutableCommand(pCommand))
    {
        // commands on the same line must be separated by ':'
        while (token == ':')
        {
            if (GetNextToken(NULL))
            {
                next = (Command *)calloc(1, sizeof(Command));
                if (next)
                {
                    next->lineNum = lineNum;
                    if (IsExecutableCommand(next))
                    {
                        pCommand->next = next;
                        pCommand = next;
                    }
                    else
                    {
                        free(next);
                        return false;
                    }
                }
                else
                {
                    Panic("system error: memory allocation error while parsing command line\n");
                    return false;
                }
            }
            else
            {
                break;
            }
        }
        if (token == EOL)
        {
            return true;
        }
    }
    
    return false;
}

bool IsExecutableCommand(Command *pCommand)
{
    if (
        IsNop(pCommand)              ||
        IsPrint(pCommand)            ||
        IsAssign(pCommand)           ||
        IsFor(pCommand)              ||
        IsNext(pCommand)             ||
        IsGoto(pCommand)             ||
        IsIf(pCommand)               ||
        IsGosub(pCommand)            ||
        IsReturn(pCommand)           ||
        IsEnd(pCommand)              ||
        IsInput(pCommand)            ||
        IsPoke(pCommand)             ||
        IsTone(pCommand)             ||
        IsBeep(pCommand)             ||
        IsLeds(pCommand)             ||
        IsDisplay(pCommand)          ||
        IsPutchar(pCommand)          ||
        IsClear(pCommand)            ||
        IsText(pCommand)             ||
        IsGr(pCommand)               ||
        IsOutchar(pCommand)          ||
        IsRseed(pCommand)            ||
        IsDelay(pCommand)            ||
        IsDim(pCommand)              ||
        IsBreak(pCommand)
    )
    {
        return true;
    }

    return false;
}

// nop
bool IsNop(Command *pCommand)
{
    return (token == EOL);
}
    
// print
bool IsPrint(Command *pCommand)
{
    PrintCommand cmd = {0};
    int printListIdx = 0;

    // PRINT expr-list
    if (token == PRINT || token == '?')
    {
        if (GetNextToken(NULL) && IsExprList(&cmd, &printListIdx))
        {
            pCommand->type = CT_PRINT;
            pCommand->cmd.printCmd = cmd;
            pCommand->cmd.printCmd.printListIdx = printListIdx;
            pCommand->cmd.printCmd.style = PS_DECIMAL;
            return true;
        }
    }

    else if (token == PRINTX)
    {
        if (GetNextToken(NULL) && IsExpr(&cmd.printList[0].expr))
        {
            pCommand->type = CT_PRINT;
            pCommand->cmd.printCmd = cmd;
            pCommand->cmd.printCmd.printListIdx = 1;
            pCommand->cmd.printCmd.style = PS_HEX;
            return true;
        }
    }

    else if (token == PRINTA)
    {
        if (GetNextToken(NULL) && IsExpr(&cmd.printList[0].expr))
        {
            pCommand->type = CT_PRINT;
            pCommand->cmd.printCmd = cmd;
            pCommand->cmd.printCmd.printListIdx = 1;
            pCommand->cmd.printCmd.style = PS_ASCII;
            return true;
        }
    }

    return false;
}

// expr-list : expr  [{';' | ','} expr-list]
bool IsExprList(PrintCommand *cmd, int *listIdx)
{
    if (!IsExpr(&cmd->printList[*listIdx].expr))
    {
        return false;
    }
    (*listIdx)++;

    // [';' | ','] expr-list
    if (token == ';' || token == ',')
    {
        cmd->printList[*listIdx].separator = token;
        if (GetNextToken(NULL))
        {
            if (!IsExprList(cmd, listIdx))
            {
                return false;
            }
        }
    }

    return true;
}

// parse the indeces of an array variable
// this MUST parse the given number of indeces
bool ParseIndeces(Node *indexNodes[], int dim)
{
    int nodeIdx = 0;

    if (token == '(')
    {
        // this is an array reference so get the first index node
        if (GetNextToken(NULL) && IsExpr(&indexNodes[nodeIdx++]))
        {
            // get the rest of the index nodes if any
            for (int i = 1; i < dim; i++)
            {
                if (token == ',')
                {
                    if (!GetNextToken(NULL) || !IsExpr(&indexNodes[nodeIdx++]))
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            if (token == ')')
            {
                return true;
            }
        }
    }

    return false;
}

// assignment
bool IsAssign(Command *pCommand)
{
    // allow "let" although unnecessary
    if (token == LET)
    {
        if (!GetNextToken(NULL))
        {
            return false;
        }
    }

    // the LHS is a symbol that represents an integer scaler or vector
    pCommand->type = CT_ASSIGN;
    pCommand->cmd.assignCmd.varsym = lexval.lexsym;

    // [let] [Numvar | Strvar] ['(' expr [',' expr]* ')'] '=' expr
    // [let] Strvar ['(' expr [',' expr]* ')'] '=' String | postfixExpr
    if (token == Numvar || token == Strvar)
    {
        // parse the index nodes for arrays
        if (SYM_DIM(pCommand->cmd.assignCmd.varsym) > 0)
        {
            if (!GetNextToken(NULL) || !ParseIndeces(pCommand->cmd.assignCmd.indexNodes, SYM_DIM(pCommand->cmd.assignCmd.varsym)))
            {
                strcpy(errorStr,"subscript error");
                return false;
            }
        }

        // evaluate the RHS
        if (GetNextToken(NULL) && token == '=')
        {
            // can only assign values to variables
            if (SYM_TYPE(pCommand->cmd.assignCmd.varsym) == ST_NUMVAR || SYM_TYPE(pCommand->cmd.assignCmd.varsym) == ST_STRVAR)
            {
                // the RHS is any primary expression
                if (GetNextToken(NULL) && IsExpr(&pCommand->cmd.assignCmd.expr))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

// for : FOR Numvar '=' expr TO expr [STEP expr]
bool IsFor(Command *pCommand)
{
    ForCommand cmd = {0};

    if (token == FOR)
    {
        if (GetNextToken(NULL) && (token == Numvar))
        {
            cmd.symbol = lexval.lexsym;
            if (GetNextToken(NULL) && (token == '='))
            {
                if (GetNextToken(NULL) && IsExpr(&cmd.init))
                {
                    if (token == TO)
                    {
                        if (GetNextToken(NULL) && IsExpr(&cmd.to))
                        {
                            if (token == STEP)
                            {
                                if (!GetNextToken(NULL) || !IsExpr(&cmd.step))
                                {
                                    return false;
                                }
                            }
                            cmd.lineNum = pCommand->lineNum;
                            pCommand->type = CT_FOR;
                            pCommand->cmd.forCmd = cmd;
                            return true;
                        }
                    }
                }
            }
        } 
    }

    return false;       
}

// next : NEXT [IntvarName]
bool IsNext(Command *pCommand)
{
    if (token == NEXT)
    {
        pCommand->cmd.nextCmd.symbol = NULL;
        if (GetNextToken(NULL) && (token == Numvar))
        {
            pCommand->cmd.nextCmd.symbol = lexval.lexsym;
        }
        pCommand->cmd.nextCmd.lineNum = pCommand->lineNum;
        pCommand->type = CT_NEXT;
        return GetNextToken(NULL);
    }

    return false;
}

// goto : GOTO Constant
bool IsGoto(Command *pCommand)
{
    Node *dest;
    
    if (token == GOTO)
    {
        if (GetNextToken(NULL) && IsExpr(&dest))
        {
            pCommand->type = CT_GOTO;
            pCommand->cmd.gotoCmd.dest = dest;
            return true;
        }
    }        

    return false;
}

// if : IF expr THEN command-list
bool IsIf(Command *pCommand)
{
    Node *expr;
    
    if (token == IF)
    {
        if (GetNextToken(NULL) && IsExpr(&expr))
        {
            // allow "then" although unnecessary
            if (token == THEN)
            {
                if (!GetNextToken(NULL))
                    return false;
            }            
            if (IsCommandList(&pCommand->cmd.ifCmd.commandList, pCommand->lineNum))
            {
                pCommand->type = CT_IF;
                pCommand->cmd.ifCmd.expr = expr;
                return true;
            }
        }
    }        

    return false;
}

// gosub : GOSUB Constant
bool IsGosub(Command *pCommand)
{
    Node *dest;
    
    if (token == GOSUB)
    {
        if (GetNextToken(NULL) && IsExpr(&dest))
        {
            pCommand->type = CT_GOSUB;
            pCommand->cmd.gosubCmd.lineNum = pCommand->lineNum;
            pCommand->cmd.gosubCmd.dest = dest;
            return true;
        }
    }        

    return false;
}

// return : RETURN
bool IsReturn(Command *pCommand)
{
    if (token == RETURN)
    {
        pCommand->type = CT_RETURN;
        return GetNextToken(NULL);
    }
    
    return false;
}

// end : END
bool IsEnd(Command *pCommand)
{
    if (token == END || token == STOP)
    {
        pCommand->type = CT_END;
        return GetNextToken(NULL);
    }
    
    return false;
}

// input : INPUT {Numvar | Strvar} ['(' expr [',' expr]* ')']
bool IsInput(Command *pCommand)
{
    if (token == INPUT)
    {
        pCommand->type = CT_INPUT;
        if (GetNextToken(NULL))
        {
            pCommand->cmd.inputCmd.varsym = lexval.lexsym;
            if (token == Numvar || token == Strvar)
            {
                // parse the index nodes for arrays
                if (SYM_DIM(pCommand->cmd.assignCmd.varsym) > 0)
                {
                    // parse the index nodes for arrays
                    if (!GetNextToken(NULL) || !ParseIndeces(pCommand->cmd.assignCmd.indexNodes, SYM_DIM(pCommand->cmd.assignCmd.varsym)))
                    {
                        strcpy(errorStr,"subscript error");
                        return false;
                    }
                }
                return GetNextToken(NULL);
            }
        }
    }
    
    return false;
}

// poke : POKE expr ',' expr
bool IsPoke(Command *pCommand)
{
    Node *addr, *data;
    
    if (token == POKE)
    {
        if (GetNextToken(NULL) && IsExpr(&addr))
        {
            if (token == ',')
            {
                if (GetNextToken(NULL) && IsExpr(&data))
                {
                    pCommand->type = CT_POKE;
                    pCommand->cmd.platformCmd.arg1 = addr;
                    pCommand->cmd.platformCmd.arg2 = data;
                    return true;
                }
            }
        }
    }
    
    return false;
}

// tone : TONE expr [',' expr]
bool IsTone(Command *pCommand)
{
    Node *freq, *duration;
    
    if (token == TONE)
    {
        if (GetNextToken(NULL) && IsExpr(&freq))
        {
            if (token == ',')
            {
                if (!GetNextToken(NULL) || !IsExpr(&duration))
                {
                    return false;
                }
            }
            else
            {
                // contrive a constant without using the lexer
                strcpy(tokenStr, "0");
                token = Constant;
                if (!SymLookup(token) || !IsPrimaryExpr(&duration))
                {
                    return false;
                }
            }
            pCommand->type = CT_TONE;
            pCommand->cmd.platformCmd.arg1 = freq;
            pCommand->cmd.platformCmd.arg2 = duration;
            return true;
        }
    }
    
    return false;
}

// beep : BEEP
bool IsBeep(Command *pCommand)
{
    if (token == BEEP)
    {
        pCommand->type = CT_BEEP;
        return GetNextToken(NULL);
    }
    
    return false;
}

// leds : LEDS expr
bool IsLeds(Command *pCommand)
{
    Node *value;
    
    if (token == LEDS)
    {
        if (GetNextToken(NULL) && IsExpr(&value))
        {
            pCommand->type = CT_LEDS;
            pCommand->cmd.platformCmd.arg1 = value;
            return true;
        }
    }
    
    return false;
}

// display : DISPLAY expr ',' expr
bool IsDisplay(Command *pCommand)
{
    // note: display quantity is either 2 or 4 (the number of 7-seg displays)
    Node *value, *displayQty;
    
    if (token == DISPLAY)
    {
        if (GetNextToken(NULL) && IsExpr(&value))
        {
            if (token == ',')
            {
                if (GetNextToken(NULL) && IsExpr(&displayQty))
                {
                    pCommand->type = CT_DISPLAY;
                    pCommand->cmd.platformCmd.arg1 = value;
                    pCommand->cmd.platformCmd.arg2 = displayQty;
                    return true;
                }
            }
        }
    }
    
    return false;
}

// putchar : PUTCHAR expr ',' expr ',' expr
bool IsPutchar(Command *pCommand)
{
    Node *row, *col, *value;
    
    if (token == PUTCHAR)
    {
        if (GetNextToken(NULL) && IsExpr(&row))
        {
            if (token == ',')
            {
                if (GetNextToken(NULL) && IsExpr(&col))
                {
                    if (token == ',')
                    {
                        if (GetNextToken(NULL) && IsExpr(&value))
                        {
                            pCommand->type = CT_PUTCHAR;
                            pCommand->cmd.platformCmd.arg1 = row;
                            pCommand->cmd.platformCmd.arg2 = col;
                            pCommand->cmd.platformCmd.arg3 = value;
                            return true;
                        }
                    }
                }
            }
        }
    }
    
    return false;
}

// clear : CLEAR
bool IsClear(Command *pCommand)
{
    if (token == CLEAR)
    {
        pCommand->type = CT_CLEAR;
        return GetNextToken(NULL);
    }
    
    return false;
}

// text : TEXT
bool IsText(Command *pCommand)
{
    if (token == TEXT)
    {
        pCommand->type = CT_TEXT;
        return GetNextToken(NULL);
    }
    
    return false;
}

// gr : GR
bool IsGr(Command *pCommand)
{
    if (token == GR)
    {
        pCommand->type = CT_GR;
        return GetNextToken(NULL);
    }
    
    return false;
}

// outchar : OUTCHAR expr
bool IsOutchar(Command *pCommand)
{
    Node *outputChar;
    
    if (token == OUTCHAR)
    {
        if (GetNextToken(NULL) && IsExpr(&outputChar))
        {
            pCommand->type = CT_OUTCHAR;
            pCommand->cmd.platformCmd.arg1 = outputChar;
            return true;
        }
    }
    
    return false;
}

// rseed : RSEED expr
bool IsRseed(Command *pCommand)
{
    Node *seed;
    
    if (token == RSEED)
    {
        if (GetNextToken(NULL) && IsExpr(&seed))
        {
            pCommand->type = CT_RSEED;
            pCommand->cmd.platformCmd.arg1 = seed;
            return true;
        }
    }
    
    return false;
}

// delay : DELAY expr
bool IsDelay(Command *pCommand)
{
    Node *duration;
    
    if (token == DELAY)
    {
        if (GetNextToken(NULL) && IsExpr(&duration))
        {
            pCommand->type = CT_DELAY;
            pCommand->cmd.platformCmd.arg1 = duration;
            return true;
        }
    }
    
    return false;
}

// DIM {Numvar | Strvar} '(' expr [',' expr]* ')'
bool IsDim(Command *pCommand)
{
    int dim = 0;
    
    if (token == DIM)
    {
        pCommand->type = CT_DIM;
        if (GetNextToken(NULL) && (token == Numvar || token == Strvar))
        {
            pCommand->cmd.dimCmd.varsym = lexval.lexsym;
            if (GetNextToken(NULL) && (token == '('))
            {
                // get the first index
                if (GetNextToken(NULL) && IsExpr(&pCommand->cmd.dimCmd.dimSizeNodes[dim++]))
                {
                    // get the rest of the indeces
                    while (token == ',')
                    {
                        if (!GetNextToken(NULL) || !IsExpr(&pCommand->cmd.dimCmd.dimSizeNodes[dim++]) || dim > DIM_MAX)
                        {
                            return false;
                        } 
                    } 
                    if (token == ')')
                    {
                        SYM_DIM(pCommand->cmd.dimCmd.varsym) = dim;
                        return GetNextToken(NULL);
                    }
                }
            }
        }
    }
    
    return false;
}

// break : BREAK
bool IsBreak(Command *pCommand)
{
    if (token == BREAK)
    {
        pCommand->type = CT_BREAK;
        return GetNextToken(NULL);
    }
    
    return false;
}


// end of parser.c

