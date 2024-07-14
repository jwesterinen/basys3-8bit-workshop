/*
*   parser.c
*
*   Basic interpreter parser.
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
bool IsPrint(Command *command);
bool IsExprList(PrintCommand *cmd, int *listIdx);
bool IsAssign(Command *command);
bool IsFor(Command *command);
bool IsNext(Command *command);
bool IsGoto(Command *command);
bool IsIf(Command *command);
bool IsGosub(Command *command);
bool IsReturn(Command *command);
bool IsEnd(Command *command);
bool IsInput(Command *command);
bool IsPoke(Command *command);
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
bool IsPostfixExprPrime(Node **ppNode);
bool IsArgExprList(Node **ppNode);
bool IsPrimaryExpr(Node **ppNode);

// expression root node list used to free expression trees
Node *ExprList[TABLE_LEN];
int exprListIdx;
extern int nodeCount;

/*
    BASIC grammar:
    
    command         : directive | deferred-cmd | immediate-cmd
    directive       : RUN | LIST | NEW
    deferred-cmd    : [Constant] executable-cmd
    executable-cmd  : immediate-cmd | for | next | goto | if | gosub | return
    immediate-cmd   : print | assign
    print           : {PRINT | '?'} print-list
    print-list      : printable  [';' | ','] print-list | printable
    printable       : expr | String
    assign          : [let] {{IntvarName '=' expr} | {StrvarName '=' String}}
    for             : FOR Identifier '=' expr TO expr [STEP expr]
    next            : NEXT [Identifier]
    goto            : GOTO Constant
    if              : IF expr THEN [immediate-cmd | goto]
    gosub           : GOSUB Constant
    return          : RETURN
    stop            : END
    end             : END
    input           : INPUT Identifier
    poke            : POKE expr ',' expr

    expr
        :  logicExpr
        
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
	    : primaryExpr postfixExpr'
	postfixExpr'
	    | '(' argExprList ')'
        | $
	    ;

    argExprList
	    : addExpr [',' argExprList]
	    ;

    primaryExpr
        : Constant 
        | Identifier
        | '(' expr ')'
        
        
*/


// command : print | assignment | for | next | goto | if | gosub | return
bool IsCommand(Command *command, bool *isImmediate)
{
    *isImmediate = false;
    
    if (token == Constant)
    {
        // if there's a line number set the command's line number and save the command string for later printing
        command->lineNum = atoi(lexval.lexeme);
        strcpy(command->commandStr, gCommandStr);
        if (!GetNextToken(NULL))
            return false;
    }
    else
    {
        *isImmediate = true;
    }
    if (
        IsPrint(command)    ||
        IsAssign(command)   ||
        IsFor(command)      ||
        IsNext(command)     ||
        IsGoto(command)     ||
        IsIf(command)       ||
        IsGosub(command)    ||
        IsReturn(command)   ||
        IsEnd(command)      ||
        IsInput(command)    ||
        IsPoke(command)
    )
    {
        sprintf(message, "node count = %d\n", nodeCount);
        MESSAGE(message);
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
        if (GetNextToken(NULL))
        {
            if (IsExprList(&cmd, &printListIdx))
            {
                command->type = CT_PRINT;
                command->cmd.printCmd = cmd;
                command->cmd.printCmd.printListIdx = printListIdx;
                return true;
            }
        }
    }
    
    return false;
}
    
// expr-list
bool IsExprList(PrintCommand *cmd, int *listIdx)
{
    Node *expr;
    
    // Identifier '$'
    if (token == StrvarName)
    {
        // list element is a string var so use its contents
        cmd->printList[*listIdx].type = VT_STRSYM;
        cmd->printList[*listIdx].value.symbol = lexval.lexsym;
        if (!GetNextToken(NULL))
            return false;
    }
    
    // expr
    else if (IsExpr(&expr))
    {
        // list element is an expression so stash the expr tree for later evaluation
        cmd->printList[*listIdx].type = VT_EXPR;
        cmd->printList[*listIdx].value.expr = expr;
    }
    
    // String
    else if (token == String)
    {
        // list element is a literal string to simply copy it
        cmd->printList[*listIdx].type = VT_STRING;
        cmd->printList[*listIdx].value.string = lexval.lexeme;
        if (!GetNextToken(NULL))
            return false;
    }
    else
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
            if (IsExprList(cmd, listIdx))
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
   
// assignment
bool IsAssign(Command *command)
{
    Symbol *symbol;
    Node *exprTree;
    
    // allow "let" although unnecessary
    if (token == LET)
    {
        if (!GetNextToken(NULL))
            return false;
    }
            
    // [let] Identifier '=' expr
    if (token == Identifier)
    {
        // the LHS is a symbol that represents only an integer variable for now
        symbol = lexval.lexsym;
        if (GetNextToken(NULL) && (token == '='))
        {
            if (GetNextToken(NULL) && IsExpr(&exprTree))
            {
                // the RHS is an expression, e.g. const, var value, array member, or function return value
                command->type = CT_ASSIGN;
                command->cmd.assignCmd.varsym = symbol;
                command->cmd.assignCmd.type = VT_EXPR;
                command->cmd.assignCmd.value.expr = exprTree;
                return true;
            }
        }
    }
    
    // [let] StrvarName = String
    else if (token == StrvarName)
    {
        symbol = lexval.lexsym;
        if (GetNextToken(NULL) && (token == '='))
        {
            if (GetNextToken(NULL) && (token == String))
            {
                command->type = CT_ASSIGN;
                command->cmd.assignCmd.varsym = symbol;
                command->cmd.assignCmd.type = VT_STRING;
                command->cmd.assignCmd.value.string = lexval.lexeme;
                return true;
            }
        }                
    }
    
    return false;
}

// for : 
bool IsFor(Command *command)
{
    ForCommand cmd = {0};
    
    // FOR IntvarName '=' expr TO expr [STEP expr]
    if (token == FOR)
    {
        if (GetNextToken(NULL) && (token == Identifier))
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
                            cmd.lineNum = command->lineNum;
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
        if (GetNextToken(NULL) && (token == Identifier))
        {
            command->cmd.nextCmd.symbol = lexval.lexsym;
        }
        command->type = CT_NEXT;
        return true;
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
bool IsGosub(Command *command)
{
    Node *dest;
    
    if (token == GOSUB)
    {
        if (GetNextToken(NULL) && IsExpr(&dest))
        {
            command->type = CT_GOSUB;
            command->cmd.gosubCmd.lineNum = command->lineNum;
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
        return true;
    }
    
    return false;
}

// end : END
bool IsEnd(Command *command)
{
    if (token == END || token == STOP)
    {
        command->type = CT_END;
        return true;
    }
    
    return false;
}

// input : INPUT Identifier
bool IsInput(Command *command)
{
    Symbol *symbol;
    
    // INPUT {IntvarName | StrvarName}
    if (token == INPUT)
    {
        command->type = CT_INPUT;
        if (GetNextToken(NULL))
        {
            symbol = lexval.lexsym;
            command->cmd.inputCmd.varsym = symbol;
            if (token == Identifier)
            {
                command->cmd.inputCmd.type = VT_EXPR;
                return true;
            }
            else if (token == StrvarName)
            {
                command->cmd.inputCmd.type = VT_STRING;
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
                    command->cmd.pokeCmd.addr = addr;
                    command->cmd.pokeCmd.data = data;
                    return true;
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
        FreeNode(node->son);
    if (node->bro)
        FreeNode(node->bro);
    free(node);
    nodeCount--;
    MESSAGE("...free node");
}

void FreeExprTrees(void)
{
    for (int i = 0; i < exprListIdx; i++)
        FreeNode(ExprList[i]);
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
    Node *id, *son;
    
    MESSAGE("IsPostfixExpr...");
    *ppNode = NewNode(NT_POSTFIX_EXPR, (union NodeValue)0);  
      
    // primaryExpr postfixExpr'
    if (IsPrimaryExpr(&son))
    {
        AddSon(*ppNode, son);
        if (token == '(')
        {  
            // TODO: this could also be an array so need some way to disambiguate
            if ((id = SON(son)) != NULL)
            {
                // id is the identifier node which needs to have its type changed from the default
                NODE_TYPE(id) = NT_FCT;
            }
            // TODO: deal with index or arg lists later
            //if (GetNextToken(NULL) && IsArgExprList(&son))
            if (GetNextToken(NULL) && IsExpr(&son))
            {      
                
                AddSon(*ppNode, son);
                if (token == ')')
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

// function arg expression list
bool IsArgExprList(Node **ppNode)
{
    Node *son;

    MESSAGE("IsArgExprList...");
    *ppNode = NewNode(NT_ARG_EXPR_LIST, (union NodeValue)0);    
    
    // addExpr [',' argExprList]
    if (IsAddExpr(&son))
    {
        AddSon(*ppNode, son);
        if (GetNextToken(NULL) && (token == ','))
        {
            if (GetNextToken(NULL) && IsArgExprList(&son))
            {
                AddSon(*ppNode, son);
                MESSAGE("...IsArgExprList");
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

    MESSAGE("IsPrimaryExpr...");
    *ppNode = NewNode(NT_PRIMARY_EXPR, (union NodeValue)0);    
    
    // Constant
    if (token == Constant)
    {
        AddSon(*ppNode, NewNode(NT_CONSTANT, (union NodeValue)atoi(lexval.lexeme)));
        if (GetNextToken(NULL))
        {
            MESSAGE("...IsPrimaryExpr");
            return true;
        }
    }
    
    // IntvarName
    else if (token == Identifier)
    {
        // assume node is an int variable but can be changed to array or fct
        AddSon(*ppNode, NewNode(NT_INTVAR, (union NodeValue)lexval.lexsym));
        if (GetNextToken(NULL))
        {
            MESSAGE("...IsPrimaryExpr");
            return true;
        }
    }
    
    // '(' expr ')'
    else if (token == '(')
    {
        if (GetNextToken(NULL) && IsExpr(&son))
        {
            AddSon(*ppNode, son);
            if (token == ')')
            {
                if (GetNextToken(NULL))
                {
                    MESSAGE("...IsPrimaryExpr");
                    return true;
                }
            }
        }
    }
    
    FreeNode(*ppNode);    
    return false;
}

// end of parser.c

