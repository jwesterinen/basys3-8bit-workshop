/*
*   parser.c
*
*   A Basic grammar parser for a Basic language interpreter.
*
*   This module will parse a Basic language command into its internal 
*   representation for later execution.
*
    BASIC grammar:
    
    command
        : directive
        | [Constant] command-line
        | 
        ;
    directive
        : RUN
        | LIST
        | NEW
        | REBOOT
        ;
    command-line
        : executable-cmd [':' command-line]
        ;
    executable-cmd  
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
        | display
        | outchar
        | rseed
        | dim
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
        : IF expr THEN [immediate-cmd | goto]
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
    display
        : DISPLAY expr ',' expr
        ;
    outchar
        :   OUTCHAR expr
        ;
    rseed
        :   RSEED expr
        ;
    delay
        :   DELAY expr
        ;
    dim
        : DIM {Numvar | Strvar} '(' expr [',' expr]+ ')'
        ;

    expr
        :  logicExpr
        ;
        
    logicExpr
	    : relExpr logicExpr'
	    ;
    logicExpr'
	    : AND_OP relExpr logicExpr'
	    | OR_OP  relExpr logicExpr'
	    | XOR_OP relExpr logicExpr'
	    | $
	    ;
	
    relExpr
	    : shiftExpr relExpr'
	    ;
    relExpr'
	    : '='   shiftExpr relExpr'
	    | NE_OP shiftExpr relExpr'
	    | '>'   shiftExpr relExpr'
	    | GE_OP shiftExpr relExpr'
	    | '<'   shiftExpr relExpr'
	    | LE_OP shiftExpr relExpr'
	    | $
	    ;
	
    shiftExpr
	    : addExpr shiftExpr'
	    ;
    shiftExpr'
	    : '='   addExpr shiftExpr'
	    | NE_OP addExpr shiftExpr'
	    | '>'   addExpr shiftExpr'
	    | GE_OP addExpr shiftExpr'
	    | '<'   addExpr shiftExpr'
	    | LE_OP addExpr shiftExpr'
	    | $
	    ;
	
    addExpr        
        : multExpr addExpr'    
    addExpr'       
        : '+' multExpr addExpr' 
        | '-' multExpr addExpr'
        | $
        ;
                    
    multExpr
        : unaryExpr multExpr'
    multExpr'
        : '*'    unaryExpr multExpr'
        | '/'    unaryExpr multExpr'
        | '%'    unaryExpr multExpr'
        | MOD_OP unaryExpr multExpr'
        | $
        ;

    unaryExpr
	    : postfixExpr
	    | ['+' | '-' | '~' | NOT_OP] unaryExpr
	    ;

    postfixExpr
	    : primaryExpr ['(' subExprList ')']
        | $
	    ;

    subExprList
	    : addExpr [',' argExprList]
	    ;

    primaryExpr
        : Constant 
        | Numvar
        | Function
        | Strvar
        | String
        | '(' expr ')'
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
#include "runtime.h"
#include "parser.h"
#include "main.h"

char *Type2Name(enum NodeType type);
Node *NewNode(enum NodeType type, union NodeValue value);
Node *AddSon(Node *parent, Node *node);
bool IsExecutableCommand(Command *command, int lineNum);
bool IsNop(Command *command);
bool IsPrint(Command *command);
bool IsExprList(PrintCommand *cmd, int *listIdx);
bool IsAssign(Command *command);
bool IsFor(Command *command, int lineNum);
bool IsNext(Command *command);
bool IsGoto(Command *command);
bool IsIf(Command *command);
bool IsGosub(Command *command, int lineNum);
bool IsReturn(Command *command);
bool IsEnd(Command *command);
bool IsInput(Command *command);
bool IsPoke(Command *command);
bool IsTone(Command *command);
bool IsBeep(Command *command);
bool IsDisplay(Command *command);
bool IsOutchar(Command *command);
bool IsRseed(Command *command);
bool IsDelay(Command *command);
bool IsDim(Command *command);
bool IsExpr(Node **ppNode);
bool IsLogicExpr(Node **ppNode);
bool IsLogicExprPrime(Node **ppNode);
bool IsRelExpr(Node **ppNode);
bool IsRelExprPrime(Node **ppNode);
bool IsShiftExpr(Node **ppNode);
bool IsShiftExprPrime(Node **ppNode);
bool IsAddExpr(Node **ppNode);
bool IsAddExprPrime(Node **ppNode);
bool IsMultExpr(Node **ppNode);
bool IsMultExprPrime(Node **ppNode);
bool IsUnaryExpr(Node **ppNode);
bool IsPostfixExpr(Node **ppNode);
bool IsSubExprList(Node **ppNode);
bool IsPrimaryExpr(Node **ppNode);

// expression root node list used to free expression trees
Node *ExprList[TABLE_LEN];
int exprListIdx;
extern int nodeCount;

// command : [Constant] command-line
bool IsCommandLine(CommandLine *commandLine, bool *isImmediate)
{
    Command *command = &commandLine->cmd, *nextCommand;
    
    // check for a line number for the command list
    if (token == Constant)
    {
        // if there's a line number set the command's line number and save the command string for later printing
        commandLine->lineNum = atoi(lexval.lexeme);
        strcpy(commandLine->commandStr, gCommandStr);
        *isImmediate = false;
        if (!GetNextToken(NULL))
        {
            return false;
        }
    }
    else
    {
        *isImmediate = true;
    }

    // create a possible list of commands
    if (IsExecutableCommand(command, commandLine->lineNum))
    {
        // commands on the same line must be separated by ':'
        while (token == ':')
        {
            if (GetNextToken(NULL))
            {
                nextCommand = (Command *)calloc(1, sizeof(Command));
                if (nextCommand)
                {
                    if (IsExecutableCommand(nextCommand, commandLine->lineNum))
                    {
                        command->next = nextCommand;
                        command = nextCommand;
                    }
                    else
                    {
                        free(nextCommand);
                        return false;
                    }
                }
                else
                {
                    strcpy(errorStr, "memory allocation error");
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

// command : print | assignment | for | next | goto | if | gosub | return
bool IsExecutableCommand(Command *command, int lineNum)
{
    if (
        IsNop(command)              ||
        IsPrint(command)            ||
        IsAssign(command)           ||
        IsFor(command, lineNum)     ||
        IsNext(command)             ||
        IsGoto(command)             ||
        IsIf(command)               ||
        IsGosub(command, lineNum)   ||
        IsReturn(command)           ||
        IsEnd(command)              ||
        IsInput(command)            ||
        IsPoke(command)             ||
        IsTone(command)             ||
        IsBeep(command)             ||
        IsDisplay(command)          ||
        IsOutchar(command)          ||
        IsRseed(command)            ||
        IsDelay(command)            ||
        IsDim(command)
    )
    {
        sprintf(message, "node count = %d\n", nodeCount);
        MESSAGE(message);
        //return GetNextToken(NULL);
        return true;
    }

    return false;
}

// nop
bool IsNop(Command *command)
{
    // NOP
    if (token == EOL)
    {
        command->type = CT_NOP;
        return true;
    }

    return false;
}
    
// print
bool IsPrint(Command *command)
{
    PrintCommand cmd = {0};
    int printListIdx = 0;

    // PRINT expr-list
    if (token == PRINT || token == '?')
    {
        if (GetNextToken(NULL) && IsExprList(&cmd, &printListIdx))
        {
            command->type = CT_PRINT;
            command->cmd.printCmd = cmd;
            command->cmd.printCmd.printListIdx = printListIdx;
            command->cmd.printCmd.style = PS_DECIMAL;
            return true;
        }
    }

    // {PRINTX | '?X'} expr
    else if (token == PRINTX)
    {
        if (GetNextToken(NULL) && IsExpr(&cmd.printList[0].expr))
        {
            command->type = CT_PRINT;
            command->cmd.printCmd = cmd;
            command->cmd.printCmd.printListIdx = 1;
            command->cmd.printCmd.style = PS_HEX;
            return true;
        }
    }

    // {PRINTA | '?A'} expr
    else if (token == PRINTA)
    {
        if (GetNextToken(NULL) && IsExpr(&cmd.printList[0].expr))
        {
            command->type = CT_PRINT;
            command->cmd.printCmd = cmd;
            command->cmd.printCmd.printListIdx = 1;
            command->cmd.printCmd.style = PS_ASCII;
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
bool ParseIndeces(Node *indexNodes[], int mod)
{
    int nodeIdx = 0;

    if (mod > 0)
    {
        if (token == '(')
        {
            // this is an array reference so get the first index node
            if (GetNextToken(NULL) && IsExpr(&indexNodes[nodeIdx++]))
            {
                // get the rest of the index nodes if any
                for (int i = 1; i < mod; i++)
                {
                    if (token == ',')
                    {
                        if (!GetNextToken(NULL) || !IsExpr(&indexNodes[nodeIdx++]))
                        {
                            return false;
                        }
                    }
                }
                if (token != ')')
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}

// assignment
bool IsAssign(Command *command)
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
    command->type = CT_ASSIGN;
    command->cmd.assignCmd.varsym = lexval.lexsym;

    // [let] Numvar ['(' expr [',' expr]* ')'] '=' expr
    // [let] Strvar ['(' expr [',' expr]* ')'] '=' String | postfixExpr
    if (token == Numvar || token == Strvar)
    {
        // parse the index nodes for arrays
        if (SYM_DIM(command->cmd.assignCmd.varsym) > 0)
        {
            if (!GetNextToken(NULL) || !ParseIndeces(command->cmd.assignCmd.indexNodes, SYM_DIM(command->cmd.assignCmd.varsym)))
            {
                return false;
            }
        }

        // perform the assignment
        if (GetNextToken(NULL))
        {
            if (token == '=')
            {
                // perform the assignment based on the LHS
                if (SYM_TYPE(command->cmd.assignCmd.varsym) == ST_NUMVAR)
                {
                    // the RHS is a const, scaler or vector number variable or function return value
                    if (GetNextToken(NULL) && IsExpr(&command->cmd.assignCmd.expr))
                    {
                        return true;
                    }
                }
                else if (SYM_TYPE(command->cmd.assignCmd.varsym) == ST_STRVAR)
                {
                    // the RHS is a string or scaler or vector string variable
                    if (GetNextToken(NULL) && IsPostfixExpr(&command->cmd.assignCmd.expr))
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

// for : 
bool IsFor(Command *command, int lineNum)
{
    ForCommand cmd = {0};

    // FOR IntvarName '=' expr TO expr [STEP expr]
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
                            cmd.lineNum = lineNum;
                            command->type = CT_FOR;
                            command->cmd.forCmd = cmd;
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
bool IsNext(Command *command)
{
    if (token == NEXT)
    {
        command->cmd.nextCmd.symbol = NULL;
        if (GetNextToken(NULL) && (token == Numvar))
        {
            command->cmd.nextCmd.symbol = lexval.lexsym;
        }
        command->type = CT_NEXT;
        return GetNextToken(NULL);
    }

    return false;
}

// goto : GOTO Constant
bool IsGoto(Command *command)
{
    Node *dest;
    
    if (token == GOTO)
    {
        if (GetNextToken(NULL) && IsExpr(&dest))
        {
            command->type = CT_GOTO;
            command->cmd.gotoCmd.dest = dest;
            return true;
        }
    }        

    return false;
}

// if
bool IsIf(Command *command)
{
    Node *expr;
    Command subCommand;
    
    // IF expr THEN [assign | print | goto]
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
            
            // TODO: allow a list of commands in an if clause
            // parse the possible commands that follow an if
            if (IsAssign(&subCommand))
            {
                command->type = CT_IF;
                command->cmd.ifCmd.expr = expr;
                command->cmd.ifCmd.type = IT_ASSIGN;
                command->cmd.ifCmd.cmd.assignCmd = subCommand.cmd.assignCmd;
                return true;
            }
            else if (IsPrint(&subCommand))
            {
                command->type = CT_IF;
                command->cmd.ifCmd.expr = expr;
                command->cmd.ifCmd.type = IT_PRINT;
                command->cmd.ifCmd.cmd.printCmd = subCommand.cmd.printCmd;
                return true;
            }
            else if (IsGoto(&subCommand))
            {
                command->type = CT_IF;
                command->cmd.ifCmd.expr = expr;
                command->cmd.ifCmd.type = IT_GOTO;
                command->cmd.ifCmd.cmd.gotoCmd = subCommand.cmd.gotoCmd;
                return true;
            }
        }
    }        

    return false;
}

// gosub : GOSUB Constant
bool IsGosub(Command *command, int lineNum)
{
    Node *dest;
    
    if (token == GOSUB)
    {
        if (GetNextToken(NULL) && IsExpr(&dest))
        {
            command->type = CT_GOSUB;
            command->cmd.gosubCmd.lineNum = lineNum;
            command->cmd.gosubCmd.dest = dest;
            return true;
        }
    }        

    return false;
}

// return : RETURN
bool IsReturn(Command *command)
{
    if (token == RETURN)
    {
        command->type = CT_RETURN;
        return GetNextToken(NULL);
    }
    
    return false;
}

// end : END
bool IsEnd(Command *command)
{
    if (token == END || token == STOP)
    {
        command->type = CT_END;
        return GetNextToken(NULL);
    }
    
    return false;
}

// input
bool IsInput(Command *command)
{
    // INPUT {Numvar | Strvar} ['(' expr [',' expr]* ')']
    if (token == INPUT)
    {
        command->type = CT_INPUT;
        if (GetNextToken(NULL))
        {
            command->cmd.inputCmd.varsym = lexval.lexsym;
            if (token == Numvar || token == Strvar)
            {
                // parse the index nodes for arrays
                if (!GetNextToken(NULL) || !ParseIndeces(command->cmd.assignCmd.indexNodes, SYM_DIM(command->cmd.assignCmd.varsym)))
                {
                    return false;
                }
                return true;
            }
        }
    }
    
    return false;
}

bool IsPoke(Command *command)
{
    Node *addr, *data;
    
    // poke : POKE expr ',' expr
    if (token == POKE)
    {
        if (GetNextToken(NULL) && IsExpr(&addr))
        {
            if (token == ',')
            {
                if (GetNextToken(NULL) && IsExpr(&data))
                {
                    command->type = CT_POKE;
                    command->cmd.platformCmd.arg1 = addr;
                    command->cmd.platformCmd.arg2 = data;
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool IsTone(Command *command)
{
    Node *freq, *duration;
    
    // tone : TONE expr [',' expr]
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
            command->type = CT_TONE;
            command->cmd.platformCmd.arg1 = freq;
            command->cmd.platformCmd.arg2 = duration;
            return true;
        }
    }
    
    return false;
}

bool IsBeep(Command *command)
{
    // beep : BEEP
    if (token == BEEP)
    {
        command->type = CT_BEEP;
        return GetNextToken(NULL);
    }
    
    return false;
}

bool IsDisplay(Command *command)
{
    Node *value, *displayQty;
    
    // display : DISPLAY expr ',' expr
    if (token == DISPLAY)
    {
        if (GetNextToken(NULL) && IsExpr(&value))
        {
            if (token == ',')
            {
                if (GetNextToken(NULL) && IsExpr(&displayQty))
                {
                    command->type = CT_DISPLAY;
                    command->cmd.platformCmd.arg1 = value;
                    command->cmd.platformCmd.arg2 = displayQty;
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool IsOutchar(Command *command)
{
    Node *outputChar;
    
    // outchar : OUTCHAR expr
    if (token == OUTCHAR)
    {
        if (GetNextToken(NULL) && IsExpr(&outputChar))
        {
            command->type = CT_OUTCHAR;
            command->cmd.platformCmd.arg1 = outputChar;
            return true;
        }
    }
    
    return false;
}

bool IsRseed(Command *command)
{
    Node *seed;
    
    // rseed : RSEED expr
    if (token == RSEED)
    {
        if (GetNextToken(NULL) && IsExpr(&seed))
        {
            command->type = CT_RSEED;
            command->cmd.platformCmd.arg1 = seed;
            return true;
        }
    }
    
    return false;
}

bool IsDelay(Command *command)
{
    Node *duration;
    
    // delay : DELAY expr
    if (token == DELAY)
    {
        if (GetNextToken(NULL) && IsExpr(&duration))
        {
            command->type = CT_DELAY;
            command->cmd.platformCmd.arg1 = duration;
            return true;
        }
    }
    
    return false;
}

bool IsDim(Command *command)
{
    int dim = 0;
    
    // DIM {Numvar | Strvar} '(' expr [',' expr]* ')'
    if (token == DIM)
    {
        command->type = CT_DIM;
        if (GetNextToken(NULL) && (token == Numvar || token == Strvar))
        {
            command->cmd.dimCmd.varsym = lexval.lexsym;
            if (GetNextToken(NULL) && (token == '('))
            {
                // get the first index
                if (GetNextToken(NULL) && IsExpr(&command->cmd.dimCmd.dimSizeNodes[dim++]))
                {
                    // get the resst of the indeces
                    while (token == ',')
                    {
                        if (!GetNextToken(NULL) || !IsExpr(&command->cmd.dimCmd.dimSizeNodes[dim++]) || dim > DIM_MAX)
                        {
                            return false;
                        } 
                    } 
                    if (token == ')')
                    {
                        SYM_DIM(command->cmd.dimCmd.varsym) = dim;
                        return GetNextToken(NULL);
                    }
                }
            }
        }
    }
    
    return false;
}

Node *NewNode(enum NodeType type, union NodeValue value)
{
    MESSAGE("new node...");
    nodeCount++;
    Node *newNode = (Node *)calloc(1, sizeof(Node));
    newNode->type = type;
    newNode->value = value;
    return newNode;
}

void FreeNode(Node *node)
{
    if (node->son)
    {
        FreeNode(node->son);
    }
    if (node->bro)
    {
        FreeNode(node->bro);
    }
    free(node);
    nodeCount--;
    MESSAGE("...free node");
}

void FreeExprTrees(void)
{
    for (int i = 0; i < exprListIdx; i++)
    {
        FreeNode(ExprList[i]);
    }
    exprListIdx = 0;
}

Node *AddSon(Node *parent, Node *node)
{
    Node *next, *last;
    
    next = SON(parent);
    if (next == NULL)
    {
        SON(parent) = node;
        return node;
    }
    while (next != NULL)
    {
        last = next;
        next = BRO(next);
    }
    BRO(last) = node;
    
    return node;
}

// expression -- this is the top-level query for an expression
// expr : logicExpr
bool IsExpr(Node **ppNode)
{
    MESSAGE("IsExpr...");
    if (IsLogicExpr(ppNode))
    {
        ExprList[exprListIdx++] = *ppNode;
        MESSAGE("...IsExpr");
        return true;
    }
    
    return false;
}

bool IsLogicExpr(Node **ppNode)
{
    Node *son;
    
    MESSAGE("IsLogicExpr...");
    
    *ppNode = NewNode(NT_LOGIC_EXPR, (union NodeValue)0);
    
    // relExpr logicExpr'
    if (IsRelExpr(&son))
    {
        AddSon(*ppNode, son);
        if (IsLogicExprPrime(&son))
        {
            AddSon(*ppNode, son);
            MESSAGE("...IsLogicExpr");
            return true;
        }
    }
    
    FreeNode(*ppNode);    
    return false;
}

// 
bool IsLogicExprPrime(Node **ppNode)
{
    Node *son;
       
    MESSAGE("IsLogicExprPrime...");
    
    // {AND_OP | OR_OP | XOR_OP} relExpr logicExpr'
    if (token == AND_OP || token == OR_OP || token == XOR_OP)
    {
        *ppNode = NewNode(NT_LOGIC_EXPR_PRIME, (union NodeValue)0);     
        AddSon(*ppNode, NewNode(NT_BINOP, (union NodeValue)token));        
        if (GetNextToken(NULL) && IsRelExpr(&son))
        {
            AddSon(*ppNode, son);
            if (IsLogicExprPrime(&son))
            {
                AddSon(*ppNode, son);
                MESSAGE("...IsLogicExprPrime");
                return true;
            }
        }
    }
    
    // $
    else
    {
        *ppNode = 0;
        MESSAGE("...IsLogicExprPrime");
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

bool IsRelExpr(Node **ppNode)
{
    Node *son;
    
    MESSAGE("IsRelExpr...");
    *ppNode = NewNode(NT_REL_EXPR, (union NodeValue)0);
    
    // shiftExpr relExpr'
    if (IsShiftExpr(&son))
    {
        AddSon(*ppNode, son);
        if (IsRelExprPrime(&son))
        {
            AddSon(*ppNode, son);
            MESSAGE("...IsRelExpr");
            return true;
        }
    }
    
    FreeNode(*ppNode);    
    return false;
}

// 
bool IsRelExprPrime(Node **ppNode)
{
    Node *son;
       
    MESSAGE("IsRelExprPrime...");
    
    // {'=' || NE_OP || '>' || GE_OP || '<' || LE_OP } shiftExpr relExpr'
    if (token == '=' || token == NE_OP || token == '>' || token == GE_OP || token == '<' || token == LE_OP)
    {
        *ppNode = NewNode(NT_REL_EXPR_PRIME, (union NodeValue)0);     
        AddSon(*ppNode, NewNode(NT_BINOP, (union NodeValue)token));        
        if (GetNextToken(NULL) && IsShiftExpr(&son))
        {
            AddSon(*ppNode, son);
            if (IsRelExprPrime(&son))
            {
                AddSon(*ppNode, son);
                MESSAGE("...IsRelExprPrime");
                return true;
            }
        }
    }
    
    // $
    else
    {
        *ppNode = 0;
        MESSAGE("...IsRelExprPrime");
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

bool IsShiftExpr(Node **ppNode)
{
    Node *son;
    
    MESSAGE("IsShiftExpr...");
    *ppNode = NewNode(NT_REL_EXPR, (union NodeValue)0);
    
    // addExpr shiftExpr'
    if (IsAddExpr(&son))
    {
        AddSon(*ppNode, son);
        if (IsShiftExprPrime(&son))
        {
            AddSon(*ppNode, son);
            MESSAGE("...IsShiftExpr");
            return true;
        }
    }
    
    FreeNode(*ppNode);    
    return false;
}

// 
bool IsShiftExprPrime(Node **ppNode)
{
    Node *son;
       
    MESSAGE("IsShiftExprPrime...");
    
    // {SL_OP || SR_OP} addExpr shiftExpr'
    if (token == SL_OP || token == SR_OP)
    {
        *ppNode = NewNode(NT_REL_EXPR_PRIME, (union NodeValue)0);     
        AddSon(*ppNode, NewNode(NT_BINOP, (union NodeValue)token));        
        if (GetNextToken(NULL) && IsAddExpr(&son))
        {
            AddSon(*ppNode, son);
            if (IsShiftExprPrime(&son))
            {
                AddSon(*ppNode, son);
                MESSAGE("...IsShiftExprPrime");
                return true;
            }
        }
    }
    
    // $
    else
    {
        *ppNode = 0;
        MESSAGE("...IsShiftExprPrime");
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

// additive expression
bool IsAddExpr(Node **ppNode)
{
    Node *son;
    
    MESSAGE("IsAddExpr...");
    *ppNode = NewNode(NT_ADD_EXPR, (union NodeValue)0);
    
    // multExpr addExpr'
    if (IsMultExpr(&son))
    {
        AddSon(*ppNode, son);
        if (IsAddExprPrime(&son))
        {
            AddSon(*ppNode, son);
            MESSAGE("...IsAddExpr");
            return true;
        }
    }
    
    FreeNode(*ppNode);    
    return false;
}

// additive expression prime
bool IsAddExprPrime(Node **ppNode)
{
    Node *son;
    
    MESSAGE("IsAddExprPrime...");
    // {'+' | '-'} multExpr addExpr'
    if (token == '+' || token == '-')
    {
        *ppNode = NewNode(NT_ADD_EXPR_PRIME, (union NodeValue)0);    
        AddSon(*ppNode, NewNode(NT_BINOP, (union NodeValue)token));
        if (GetNextToken(NULL) && IsMultExpr(&son))
        {
            AddSon(*ppNode, son);
            if (IsAddExprPrime(&son))
            {
                AddSon(*ppNode, son);
                MESSAGE("...IsAddExprPrime");
                return true;
            }
        }
    }
    
    // $
    else
    {
        *ppNode = 0;
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

// multiplicative expression
bool IsMultExpr(Node **ppNode)
{
    Node *son;
    
    MESSAGE("IsMultExpr...");
    *ppNode = NewNode(NT_MULT_EXPR, (union NodeValue)0);  
      
    // unaryExpr multExpr'
    if (IsUnaryExpr(&son))
    {
        AddSon(*ppNode, son);
        if (IsMultExprPrime(&son))
        {
            AddSon(*ppNode, son);
            MESSAGE("...IsMultExpr");
            return true;
        }
    }
    
    FreeNode(*ppNode);
    return false;
}

// multiplicative expression prime
bool IsMultExprPrime(Node **ppNode)
{
    Node *son;
    
    MESSAGE("IsMultExprPrime...");
    // {'*' | '/' | '%' | MOD_OP} unaryExpr multExpr'
    if (token == '*' || token == '/' || token == '%' || token == MOD_OP)
    {
        *ppNode = NewNode(NT_MULT_EXPR_PRIME, (union NodeValue)0);
        
        AddSon(*ppNode, NewNode(NT_BINOP, (union NodeValue)token));
        if (GetNextToken(NULL) && IsUnaryExpr(&son))
        {
            AddSon(*ppNode, son);
            if (IsMultExprPrime(&son))
            {
                AddSon(*ppNode, son);
                MESSAGE("...IsMultExprPrime");
                return true;
            }
        }
    }
    
    // $
    else
    {
        *ppNode = 0;
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

// unary expression
bool IsUnaryExpr(Node **ppNode)
{
    Node *son;
    
    MESSAGE("IsUnaryExpr...");
    *ppNode = NewNode(NT_UNARY_EXPR, (union NodeValue)0);
    
    // always have a unop node and default it to be '+'
    son = NewNode(NT_UNOP, (union NodeValue)'+');
    AddSon(*ppNode, son);
    
	// ['+' | '-' | '~' | NOT_OP] unaryExpr
    if (token == '+' || token == '-' || token == '~' || token == NOT_OP)
    {
        // change the unary operator if there is an explicit op
        NODE_VAL_OP(son) = token;
        if (!GetNextToken(NULL))
        {
            FreeNode(*ppNode);
            return false;
        }
    }    
    if (IsPostfixExpr(&son))
    {
        AddSon(*ppNode, son);
        MESSAGE("...IsUnaryExpr");
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

// postfix expression
bool IsPostfixExpr(Node **ppNode)
{
    Node *son;
    
    MESSAGE("IsPostfixExpr...");
    *ppNode = NewNode(NT_POSTFIX_EXPR, (union NodeValue)0);
      
    // primaryExpr ['(' argExprList ')']
    if (IsPrimaryExpr(&son))
    {
        AddSon(*ppNode, son);
        if (token == '(')
        {  
            if (GetNextToken(NULL) && IsSubExprList(&son))
            {      
                AddSon(*ppNode, son);
                if (token == ')' && GetNextToken(NULL))
                {      
                    return true;
                }
            }
        }
        else
        {
            return true;
        }
    }
    
    FreeNode(*ppNode);
    return false;
}

// subscript expression list
bool IsSubExprList(Node **ppNode)
{
    Node *son;

    MESSAGE("IsSubExprList...");
    *ppNode = NewNode(NT_SUB_EXPR_LIST, (union NodeValue)0);    
    
    // addExpr [',' subExprList]
    if (IsAddExpr(&son))
    {
        AddSon(*ppNode, son);
        if (token == ',')
        {
            if (GetNextToken(NULL) && IsSubExprList(&son))
            {
                AddSon(*ppNode, son);
                return true;
            }
        }
        else
        {
            return true;
        }
    }
    
    FreeNode(*ppNode);
    return false;
}

// primary expression
bool IsPrimaryExpr(Node **ppNode)
{
    Node *son;
    float value;

    MESSAGE("IsPrimaryExpr...");
    *ppNode = NewNode(NT_PRIMARY_EXPR, (union NodeValue)0);    
    
    switch (token)
    {
        case Constant:
            if (lexval.lexeme[1] == 'x' || lexval.lexeme[1] == 'X')
            {
                value = (float)strtol(lexval.lexeme, NULL, 16);
            }
            else
            {
                value = strtod(lexval.lexeme, NULL);
            }
            AddSon(*ppNode, NewNode(NT_CONSTANT, (union NodeValue)((float)value)));
            if (GetNextToken(NULL))
            {
                MESSAGE("...IsPrimaryExpr Constant");
                return true;
            }
            break;
        
        case Numvar:
            AddSon(*ppNode, NewNode(NT_NUMVAR, (union NodeValue)lexval.lexsym));
            if (GetNextToken(NULL))
            {
                MESSAGE("...IsPrimaryExpr Numvar");
                return true;
            }
            break;
        
        case Function:
            AddSon(*ppNode, NewNode(NT_FCT, (union NodeValue)lexval.lexsym));
            if (GetNextToken(NULL))
            {
                MESSAGE("...IsPrimaryExpr Function");
                return true;
            }
            break;
        
        case Strvar:
            AddSon(*ppNode, NewNode(NT_STRVAR, (union NodeValue)lexval.lexsym));
            if (GetNextToken(NULL))
            {
                MESSAGE("...IsPrimaryExpr Strvar");
                return true;
            }
            break;
        
        case String:
            AddSon(*ppNode, NewNode(NT_STRING, (union NodeValue)lexval.lexeme));
            if (GetNextToken(NULL))
            {
                MESSAGE("...IsPrimaryExpr String");
                return true;
            }
            break;
        
        // '(' expr ')'
        case '(':
            if (GetNextToken(NULL) && IsExpr(&son))
            {
                AddSon(*ppNode, son);
                if (token == ')')
                {
                    if (GetNextToken(NULL))
                    {
                        MESSAGE("...IsPrimaryExpr expr");
                        return true;
                    }
                }
            }
            break;
    }
    
    FreeNode(*ppNode);    
    return false;
}

// end of parser.c

