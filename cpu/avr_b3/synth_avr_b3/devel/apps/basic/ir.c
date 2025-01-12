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
	    | ['+' | '-' | '~' | NOT_OP] addExpr
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

#define USE_SYNTAX_TREE   
 
Node *NewNode(enum NodeType type, union NodeValue value);
Node *AddSon(Node *parent, Node *node);
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
bool IsSubExprList(Node **ppNode, int *subExprQty);

Node *TraverseParseTree(Node *node);

// expression root node list used to free expression trees
Node *ExprList[TABLE_LEN];
int exprListIdx;

// syntax tree list used to free them
Node *SyntaxTreeList[TABLE_LEN];
int syntaxTreeListIdx;

// total node count
int gNodeQty = 0;

Node *NewNode(enum NodeType type, union NodeValue value)
{
    Node *newNode = (Node *)calloc(1, sizeof(Node));
    
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
    gNodeQty--;
}

void FreeExprTrees(void)
{
    for (int i = 0; i < exprListIdx; i++)
    {
        FreeNode(ExprList[i]);
    }
    exprListIdx = 0;
    for (int i = 0; i < syntaxTreeListIdx; i++)
    {
        FreeNode(SyntaxTreeList[i]);
    }
    syntaxTreeListIdx = 0;
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

Node *AddOpndR(Node *parent, Node *node)
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

// trim superfluous tree nodes
void TrimTree(Node *parent, Node *node)
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

Node *TraverseParseTree(Node *node)
{
    Node *stNode = NULL, *opnd, *son, *bro;
    
    if (node != NULL)
    {
        switch (NODE_TYPE(node))
        {
            case NT_LOGIC_EXPR:
                //  expr
                //    : expr
                stNode = TraverseParseTree(node->son);        // pure PM
                return stNode;
                
            case NT_REL_EXPR:
            case NT_SHIFT_EXPR:
            case NT_ADD_EXPR:
            case NT_MULT_EXPR:
                //  expr
                //    : expr expr'
                opnd = TraverseParseTree(node->son);        // left opnd or pure PM
                stNode = TraverseParseTree(node->bro);      // binop tree with right opnd
                if (stNode != NULL)
                {
                    stNode->son = opnd; 
                    return stNode;
                }
                else
                {
                    stNode = opnd;
                    return stNode;
                }
                break;
                
            case NT_LOGIC_EXPR_PRIME:
            case NT_REL_EXPR_PRIME:
            case NT_SHIFT_EXPR_PRIME:
            case NT_ADD_EXPR_PRIME:
            case NT_MULT_EXPR_PRIME:
                // binary expressions
                // expr'
	            //    : op expr expr'
	            //    ...
	            //    | $
                stNode = TraverseParseTree(node->son);      // binop node
                opnd = TraverseParseTree(node->son->bro);   // right opnd
                stNode->bro = opnd;
                return stNode;
                
            case NT_UNARY_EXPR:
                // unaryExpr
                //    : postfixExpr
                //    | ['+' | '-' | '~' | NOT_OP] addExpr
                stNode = TraverseParseTree(node->son);          // unop or postfixExpr
                if (NODE_TYPE(stNode) == NT_UNOP)
                {
                    opnd = TraverseParseTree(node->son->bro);   // opnd
                    stNode->son = opnd;
                }
                return stNode;
                break;
                
            case NT_POSTFIX_EXPR:
                // postfixExpr
                //   primaryExpr [subExprList]
                stNode = TraverseParseTree(node->son);          // PE, array/fct name or pure PE
                
                // the presence of a son's bro node indicates that this is a vector
                if (BRO(SON(node)))
                {
                    // check for attempt to use scaler as vector if this is not a builtin fct
                    if (SYM_TYPE(SON(SON(node))->value.varsym) != ST_FCT)
                    {
                        if (SYM_DIM(SON(SON(node))->value.varsym) == 0)
                        {
                            strcpy(errorStr, "subscript error: attempt to use scaler as vector");
                            return NULL;
                        }
                    }
                    
                    opnd = TraverseParseTree(BRO(SON(node)));    // arg/subscript list extracted from SEL, could be null
                    stNode->bro = opnd;
                }
                return stNode;
                
            // FIXME: there must be some kind of non-PE, non-OP placeholder node type that contains an arg/subscript expr and is understood by the syntax tree traversal,
            //        -- possibly use the NT_EXPR node type
            case NT_SUB_EXPR_LIST:
                // subExprList
                //   addExpr [',' subExprList]
                stNode = NewNode(NT_EXPR, (union NodeValue)0);  // expr placeholder node
                son = TraverseParseTree(SON(node));             // arg/subscript expr
                stNode->son = son;
                if (son)
                {
                    bro = TraverseParseTree(BRO(SON(node)));        // another SEl
                    stNode->bro = bro;
                }
                return stNode;
            
            case NT_PRIMARY_EXPR:
                // postfixExpr
                //   primaryExpr [subExprList]
                stNode = TraverseParseTree(node->son);  // const, string, numvar, strvar, fct
                return stNode;
                
            case NT_BINOP:
                switch (NODE_VAL_OP(node))
                {    
                    case AND_OP:
                    case OR_OP:
                    case XOR_OP:
                    case '=':
                    case NE_OP:
                    case '>':
                    case GE_OP:
                    case '<':
                    case LE_OP:
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                    case '%':
                    case MOD_OP:
                    case SL_OP:
                    case SR_OP:
                        stNode = NewNode(NT_BINOP, (union NodeValue)(NODE_VAL_OP(node)));
                        return stNode;
                    default:
                        return NULL;
                }
                
            case NT_UNOP:
                switch (NODE_VAL_OP(node))
                {    
                    case '+':
                    case '-':
                    case NOT_OP:
                        stNode = NewNode(NT_UNOP, (union NodeValue)(NODE_VAL_OP(node)));
                        return stNode;
                    default:
                        return NULL;
                }
                
            case NT_CONSTANT:
                stNode = NewNode(NT_CONSTANT, (union NodeValue)(NODE_VAL_CONST(node)));
                return stNode;
                
            case NT_STRING:
                stNode = NewNode(NT_STRING, (union NodeValue)(NODE_VAL_STRING(node)));
                return stNode;
                
            case NT_NUMVAR:
                stNode = NewNode(NT_NUMVAR, (union NodeValue)(NODE_VAL_VARSYM(node)));
                return stNode;
                
            case NT_STRVAR:
                stNode = NewNode(NT_STRVAR, (union NodeValue)(NODE_VAL_VARSYM(node)));
                return stNode;
                
            case NT_FCT:
                stNode = NewNode(NT_FCT, (union NodeValue)(NODE_VAL_VARSYM(node)));
                return stNode;
                
            default:
                break;
        }
    }                
    
    return stNode;
}

// expression -- this is the top-level query for an expression
// expr : logicExpr
bool IsExpr(Node **ppNode)
{
    Node *parseTree, *syntaxTree;
    
    MESSAGE("building expression tree...");
    if (IsLogicExpr(&parseTree))
    {
#ifndef USE_SYNTAX_TREE    
        // build list of expressions so they can be freed
        ExprList[exprListIdx++] = parseTree;
        
        // trim the parse tree then return it
        TrimTree(parseTree, parseTree->son);
        *ppNode = parseTree;
#else        
        // create syntax tree from parse tree
        syntaxTree = TraverseParseTree(parseTree);
        
        // add to list of syntax trees so they can be freed then free the unnecessary parse tree
        SyntaxTreeList[syntaxTreeListIdx++] = syntaxTree;        
        FreeNode(parseTree);
        
        // return the syntax tree
        *ppNode = syntaxTree;
#endif        
        
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
            //MESSAGE("...IsLogicExpr");
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
            //MESSAGE("...IsRelExpr");
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

bool IsShiftExpr(Node **ppNode)
{
    Node *son;
    
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
bool IsShiftExprPrime(Node **ppNode)
{
    Node *son;
       
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
            //MESSAGE("...IsAddExpr");
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
            //MESSAGE("...IsMultExpr");
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
//    unaryExpr
//	    : postfixExpr
//	    | ['+' | '-' | '~' | NOT_OP] addExpr
bool IsUnaryExpr(Node **ppNode)
{
    Node *son;
    int op;
    
    *ppNode = NewNode(NT_UNARY_EXPR, (union NodeValue)0);
    
	// ['+' | '-' | '~' | NOT_OP] addExpr
    if (token == '+' || token == '-' || token == '~' || token == NOT_OP)
    {
        op = token;
        if (GetNextToken(NULL) && IsAddExpr(&son))
        {
            AddSon(*ppNode, NewNode(NT_UNOP, (union NodeValue)op));
            sprintf(message, "IsUnaryExpr unop '%c'", (char)op);
            AddSon(*ppNode, son);
            return true;
        }
    }    
    // postfixExpr
    else if (IsPostfixExpr(&son))
    {
        AddSon(*ppNode, son);
        return true;
    }
    
    FreeNode(*ppNode);
    return false;
}

// postfix expression
bool IsPostfixExpr(Node **ppNode)
{
    Node *son;
    int subExprQty = 0;
    
    MESSAGE("IsPostfixExpr...");
    *ppNode = NewNode(NT_POSTFIX_EXPR, (union NodeValue)0);
      
    // primaryExpr ['(' [subExprList] ')']
    if (IsPrimaryExpr(&son))
    {
        // TODO: check for consistence in what kind of primary expr it is, i.e. can't have PFE with const or string
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
            // not a vector nor a function
            return true;
        }
    }
    
    FreeNode(*ppNode);
    return false;
}

// subscript expression list
// parse the arguments and return the number parsed
//   subExprList
//      : addExpr [',' subExprList]
//	    | $
bool IsSubExprList(Node **ppNode, int *subExprQty)
{
    Node *son;

    MESSAGE("IsSubExprList...");
    *ppNode = NewNode(NT_SUB_EXPR_LIST, (union NodeValue)0);    
    
    // addExpr [',' subExprList]
    if (IsLogicExpr(&son))
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
bool IsPrimaryExpr(Node **ppNode)
{
    Node *son;
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
        
        case String:
            AddSon(*ppNode, NewNode(NT_STRING, (union NodeValue)lexval.lexeme));
            if (GetNextToken(NULL))
            {
                sprintf(message, "IsPrimaryExpr String %s", NODE_VAL_STRING(SON(*ppNode)));
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
        
        case Strvar:
            AddSon(*ppNode, NewNode(NT_STRVAR, (union NodeValue)lexval.lexsym));
            if (GetNextToken(NULL))
            {
                sprintf(message, "IsPrimaryExpr Strvar %s", SYM_NAME(NODE_VAL_VARSYM(SON(*ppNode))));
                MESSAGE(message);
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
        
        // '(' expr ')'
        case '(':
            if (GetNextToken(NULL) && IsLogicExpr(&son))
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


// end of ir.c

