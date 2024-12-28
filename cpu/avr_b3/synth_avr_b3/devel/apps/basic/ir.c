/*
 *  This file contains the internal representation of the Basic Interpreter which consists
 *  of both the parse tree and the syntax tree.
 *
 *  The following is the grammar for expressions:
 *
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
	    : addExpr [',' subExprList]
	    | $
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

PT_Node *NewNode(enum NodeType type, union NodeValue value);
PT_Node *AddSon(PT_Node *parent, PT_Node *node);
bool IsExpr(PT_Node **ppNode);
bool IsLogicExpr(PT_Node **ppNode);
bool IsLogicExprPrime(PT_Node **ppNode);
bool IsRelExpr(PT_Node **ppNode);
bool IsRelExprPrime(PT_Node **ppNode);
bool IsShiftExpr(PT_Node **ppNode);
bool IsShiftExprPrime(PT_Node **ppNode);
bool IsAddExpr(PT_Node **ppNode);
bool IsAddExprPrime(PT_Node **ppNode);
bool IsMultExpr(PT_Node **ppNode);
bool IsMultExprPrime(PT_Node **ppNode);
bool IsUnaryExpr(PT_Node **ppNode);
bool IsSubExprList(PT_Node **ppNode, int *subExprQty);

// expression root node list used to free expression trees
PT_Node *ExprList[TABLE_LEN];
int exprListIdx;

// total node count
int gNodeQty = 0;

PT_Node *NewNode(enum NodeType type, union NodeValue value)
{
    PT_Node *newNode = (PT_Node *)calloc(1, sizeof(PT_Node));
    
    //MESSAGE("new node...");
    
    if (newNode)
    {
        newNode->type = type;
        newNode->value = value;
        gNodeQty++;
    }
    else
    {
        Panic("system error: memory allocation error while creating expression node\n");
    }
    
    return newNode;
}

void FreeNode(PT_Node *node)
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
    gNodeQty--;
}

void FreeExprTrees(void)
{
    for (int i = 0; i < exprListIdx; i++)
    {
        FreeNode(ExprList[i]);
    }
    exprListIdx = 0;
}

PT_Node *AddSon(PT_Node *parent, PT_Node *node)
{
    PT_Node *next, *last;
    
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

// trim superfluous tree nodes
void TrimTree(PT_Node *parent, PT_Node *node)
{
    if (NODE_TYPE(node) != NT_POSTFIX_EXPR)
    {
        if (node->bro == NULL)
        {
            TrimTree(node, node->son);
            parent->son = node->son;
            free(node);
            gNodeQty--;
        }
        else
        {
            TrimTree(node, node->son);
            if (node->bro->son)
            {
                TrimTree(node->bro->son->bro, node->bro->son->bro->son);
            }
        }
    }
}

// expression -- this is the top-level query for an expression
// expr : logicExpr
bool IsExpr(PT_Node **ppNode)
{
    MESSAGE("building expression tree...");
    if (IsLogicExpr(ppNode))
    {
        // build list of expressions so they can be freed
        ExprList[exprListIdx++] = *ppNode;
        
        // trim the tree
        //TrimTree(*ppNode, (*ppNode)->son);
        
        return true;
    }
    
    return false;
}

bool IsLogicExpr(PT_Node **ppNode)
{
    PT_Node *son;
        
    MESSAGE("IsLogicExpr...");
    *ppNode = NewNode(NT_LOGIC_EXPR, (union NodeValue)0);
    
    // relExpr logicExpr'
    if (IsRelExpr(&son))
    {
        AddSon(*ppNode, son);
        if (IsLogicExprPrime(&son))
        {
            AddSon(*ppNode, son);
            //MESSAGE("...IsLogicExpr");
            return true;
        }
    }
    
    FreeNode(*ppNode);    
    return false;
}

// 
bool IsLogicExprPrime(PT_Node **ppNode)
{
    PT_Node *son;
       
    
    // {AND_OP | OR_OP | XOR_OP} relExpr logicExpr'
    if (token == AND_OP || token == OR_OP || token == XOR_OP)
    {
        *ppNode = NewNode(NT_LOGIC_EXPR_PRIME, (union NodeValue)0);     
        AddSon(*ppNode, NewNode(NT_BINOP, (union NodeValue)token));        
        sprintf(message, "IsLogicExprPrime binop %c", token);
        MESSAGE(message);
        if (GetNextToken(NULL) && IsRelExpr(&son))
        {
            AddSon(*ppNode, son);
            if (IsLogicExprPrime(&son))
            {
                AddSon(*ppNode, son);
                //MESSAGE("...IsLogicExprPrime");
                return true;
            }
        }
    }
    
    // $
    else
    {
        *ppNode = 0;
        //MESSAGE("...IsLogicExprPrime");
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

bool IsRelExpr(PT_Node **ppNode)
{
    PT_Node *son;
    
    MESSAGE("IsRelExpr...");
    *ppNode = NewNode(NT_REL_EXPR, (union NodeValue)0);
    
    // shiftExpr relExpr'
    if (IsShiftExpr(&son))
    {
        AddSon(*ppNode, son);
        if (IsRelExprPrime(&son))
        {
            AddSon(*ppNode, son);
            //MESSAGE("...IsRelExpr");
            return true;
        }
    }
    
    FreeNode(*ppNode);    
    return false;
}

// 
bool IsRelExprPrime(PT_Node **ppNode)
{
    PT_Node *son;
       
    
    // {'=' || NE_OP || '>' || GE_OP || '<' || LE_OP } shiftExpr relExpr'
    if (token == '=' || token == NE_OP || token == '>' || token == GE_OP || token == '<' || token == LE_OP)
    {
        *ppNode = NewNode(NT_REL_EXPR_PRIME, (union NodeValue)0);     
        AddSon(*ppNode, NewNode(NT_BINOP, (union NodeValue)token));        
        sprintf(message, "IsRelExprPrime binop %c", token);
        MESSAGE(message);
        if (GetNextToken(NULL) && IsShiftExpr(&son))
        {
            AddSon(*ppNode, son);
            if (IsRelExprPrime(&son))
            {
                AddSon(*ppNode, son);
                //MESSAGE("...IsRelExprPrime");
                return true;
            }
        }
    }
    
    // $
    else
    {
        *ppNode = 0;
        //MESSAGE("...IsRelExprPrime");
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

bool IsShiftExpr(PT_Node **ppNode)
{
    PT_Node *son;
    
    MESSAGE("IsShiftExpr...");
    *ppNode = NewNode(NT_SHIFT_EXPR, (union NodeValue)0);
    
    // addExpr shiftExpr'
    if (IsAddExpr(&son))
    {
        AddSon(*ppNode, son);
        if (IsShiftExprPrime(&son))
        {
            AddSon(*ppNode, son);
            //MESSAGE("...IsShiftExpr");
            return true;
        }
    }
    
    FreeNode(*ppNode);    
    return false;
}

// 
bool IsShiftExprPrime(PT_Node **ppNode)
{
    PT_Node *son;
       
    // {SL_OP || SR_OP} addExpr shiftExpr'
    if (token == SL_OP || token == SR_OP)
    {
        *ppNode = NewNode(NT_SHIFT_EXPR_PRIME, (union NodeValue)0);     
        AddSon(*ppNode, NewNode(NT_BINOP, (union NodeValue)token));        
        sprintf(message, "IsShiftExprPrime binop %c", token);
        MESSAGE(message);
        if (GetNextToken(NULL) && IsAddExpr(&son))
        {
            AddSon(*ppNode, son);
            if (IsShiftExprPrime(&son))
            {
                AddSon(*ppNode, son);
                //MESSAGE("...IsShiftExprPrime");
                return true;
            }
        }
    }
    
    // $
    else
    {
        *ppNode = 0;
        //MESSAGE("...IsShiftExprPrime");
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

// additive expression
bool IsAddExpr(PT_Node **ppNode)
{
    PT_Node *son;
    
    MESSAGE("IsAddExpr...");
    *ppNode = NewNode(NT_ADD_EXPR, (union NodeValue)0);
    
    // multExpr addExpr'
    if (IsMultExpr(&son))
    {
        AddSon(*ppNode, son);
        if (IsAddExprPrime(&son))
        {
            AddSon(*ppNode, son);
            //MESSAGE("...IsAddExpr");
            return true;
        }
    }
    
    FreeNode(*ppNode);    
    return false;
}

// additive expression prime
bool IsAddExprPrime(PT_Node **ppNode)
{
    PT_Node *son;
    
    // {'+' | '-'} multExpr addExpr'
    if (token == '+' || token == '-')
    {
        *ppNode = NewNode(NT_ADD_EXPR_PRIME, (union NodeValue)0);    
        AddSon(*ppNode, NewNode(NT_BINOP, (union NodeValue)token));
        sprintf(message, "IsAddExprPrime binop %c", token);
        MESSAGE(message);
        if (GetNextToken(NULL) && IsMultExpr(&son))
        {
            AddSon(*ppNode, son);
            if (IsAddExprPrime(&son))
            {
                AddSon(*ppNode, son);
                //MESSAGE("...IsAddExprPrime");
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
bool IsMultExpr(PT_Node **ppNode)
{
    PT_Node *son;
    
    MESSAGE("IsMultExpr...");
    *ppNode = NewNode(NT_MULT_EXPR, (union NodeValue)0);  
      
    // unaryExpr multExpr'
    if (IsUnaryExpr(&son))
    {
        AddSon(*ppNode, son);
        if (IsMultExprPrime(&son))
        {
            AddSon(*ppNode, son);
            //MESSAGE("...IsMultExpr");
            return true;
        }
    }
    
    FreeNode(*ppNode);
    return false;
}

// multiplicative expression prime
bool IsMultExprPrime(PT_Node **ppNode)
{
    PT_Node *son;
    
    // {'*' | '/' | '%' | MOD_OP} unaryExpr multExpr'
    if (token == '*' || token == '/' || token == '%' || token == MOD_OP)
    {
        *ppNode = NewNode(NT_MULT_EXPR_PRIME, (union NodeValue)0);        
        AddSon(*ppNode, NewNode(NT_BINOP, (union NodeValue)token));
        sprintf(message, "IsMultExprPrime binop %c", token);
        MESSAGE(message);
        if (GetNextToken(NULL) && IsUnaryExpr(&son))
        {
            AddSon(*ppNode, son);
            if (IsMultExprPrime(&son))
            {
                AddSon(*ppNode, son);
                //MESSAGE("...IsMultExprPrime");
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
bool IsUnaryExpr(PT_Node **ppNode)
{
    PT_Node *son, *opNode = NULL;
    char op;
    
    *ppNode = NewNode(NT_UNARY_EXPR, (union NodeValue)0);
    
	// ['+' | '-' | '~' | NOT_OP] unaryExpr
    if (token == '+' || token == '-' || token == '~' || token == NOT_OP)
    {
        opNode = NewNode(NT_UNOP, (union NodeValue)'+');
        NODE_VAL_OP(opNode) = token;
        op = token;
        if (!GetNextToken(NULL))
        {
            FreeNode(*ppNode);
            return false;
        }
    }    
    if (IsPostfixExpr(&son))
    {
        AddSon(*ppNode, son);
        if (opNode)
        {
            AddSon(*ppNode, opNode);
            sprintf(message, "IsUnaryExpr unop '%c'", op);
            MESSAGE(message);
        }
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

// postfix expression
bool IsPostfixExpr(PT_Node **ppNode)
{
    PT_Node *son;
    int subExprQty = 0;
    
    MESSAGE("IsPostfixExpr...");
    *ppNode = NewNode(NT_POSTFIX_EXPR, (union NodeValue)0);
      
    // primaryExpr ['(' [subExprList] ')']
    if (IsPrimaryExpr(&son))
    {
        AddSon(*ppNode, son);
        if (token == '(')
        {  
            if (GetNextToken(NULL) && IsSubExprList(&son, &subExprQty))
            {      
                // set the postfix expr node's "op" value to the subexpr size, which could be 0
                NODE_VAL_OP(*ppNode) = subExprQty;
                
                // add the subexprlist which could be null as a son of the postfix expr node
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
// parse the arguments and return the number parsed
bool IsSubExprList(PT_Node **ppNode, int *subExprQty)
{
    PT_Node *son;

    MESSAGE("IsSubExprList...");
    *ppNode = NewNode(NT_SUB_EXPR_LIST, (union NodeValue)0);    
    
    // addExpr [',' subExprList]
    if (IsAddExpr(&son))
    {
        AddSon(*ppNode, son);
        (*subExprQty)++;
        if (token == ',')
        {
            if (GetNextToken(NULL) && IsSubExprList(&son, subExprQty))
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
    
    // $
    else
    {
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

// primary expression
bool IsPrimaryExpr(PT_Node **ppNode)
{
    PT_Node *son;
    float value;

    //MESSAGE("IsPrimaryExpr...");
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
                sprintf(message, "IsPrimaryExpr Constant %f", NODE_VAL_CONST(SON(*ppNode)));
                MESSAGE(message);
                return true;
            }
            break;
        
        case Numvar:
            AddSon(*ppNode, NewNode(NT_NUMVAR, (union NodeValue)lexval.lexsym));
            if (GetNextToken(NULL))
            {
                sprintf(message, "IsPrimaryExpr Numvar %s", SYM_NAME(NODE_VAL_VARSYM(SON(*ppNode))));
                return true;
            }
            break;
        
        case Function:
            AddSon(*ppNode, NewNode(NT_FCT, (union NodeValue)lexval.lexsym));
            if (GetNextToken(NULL))
            {
                sprintf(message, "IsPrimaryExpr Function %s", SYM_NAME(NODE_VAL_VARSYM(SON(*ppNode))));
                MESSAGE(message);
                return true;
            }
            break;
        
        case Strvar:
            AddSon(*ppNode, NewNode(NT_STRVAR, (union NodeValue)lexval.lexsym));
            if (GetNextToken(NULL))
            {
                sprintf(message, "IsPrimaryExpr Strvar %s", SYM_NAME(NODE_VAL_VARSYM(SON(*ppNode))));
                MESSAGE(message);
                return true;
            }
            break;
        
        case String:
            AddSon(*ppNode, NewNode(NT_STRING, (union NodeValue)lexval.lexeme));
            if (GetNextToken(NULL))
            {
                sprintf(message, "IsPrimaryExpr String %s", NODE_VAL_STRING(SON(*ppNode)));
                MESSAGE(message);
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
                        MESSAGE("...IsPrimaryExpr EXPR");
                        return true;
                    }
                }
            }
            break;
    }
    
    FreeNode(*ppNode);    
    return false;
}


