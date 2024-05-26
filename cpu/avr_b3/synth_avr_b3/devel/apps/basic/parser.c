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
#include "parser.h"
#include "lexer.h"
#include "eval_stack.h"

enum NodeType {
    NT_NONE = 0,
    NT_INSTR,
    NT_PRINT, NT_ASSIGN, NT_EXPR_LIST,
    NT_EXPR, NT_EXPR_PRIME, NT_TERM, NT_TERM_PRIME, NT_FACTOR, 
    NT_ADD_OP, NT_SUB_OP, NT_MUL_OP, NT_DIV_OP,
    NT_CONSTANT, NT_IDENTIFIER
};
typedef uint8_t NodeID;
typedef struct Node {
    enum NodeType type;
    int constant;
    SymbolID symbol;
    NodeID bro;
    NodeID son;
} Node;

#define TYPE(node)      nodetab[(node)].type
#define CONSTANT(node)  nodetab[(node)].constant
#define SYMBOL(node)    nodetab[(node)].symbol
#define BRO(node)       nodetab[(node)].bro
#define SON(node)       nodetab[(node)].son
#define SYMNAME(symbol) symtab[(symbol)].name
#define SYMVAL(symbol)  symtab[(symbol)].value
#define SYMDEF(symbol)  symtab[(symbol)].isDefined

char *Type2Name(enum NodeType type);
NodeID NewNode(enum NodeType type);
NodeID AddSon(NodeID parent, NodeID node);
bool IsInstr(NodeID *pNode);
bool IsPrint(NodeID *pNode);
bool IsExprList(NodeID *pNode);
bool IsAssign(NodeID *pNode);
bool IsExpr(NodeID *pNode);
bool IsExprPrime(NodeID *pNode);
bool IsTerm(NodeID *pNode);
bool IsTermPrime(NodeID *pNode);
bool IsFactor(NodeID *pNode);

char errorStr[80];
char resultStr[80];

Node nodetab[100];
int nodetabIdx = 1;

char *Type2Name(enum NodeType type)
{
    switch (type)
    {
        case NT_INSTR:      return "instr";     break;
        case NT_PRINT:      return "print";     break;
        case NT_ASSIGN:     return "assign";    break;
        case NT_EXPR:       return "expr";      break;
        case NT_EXPR_PRIME: return "expr'";     break;
        case NT_TERM:       return "term";      break;
        case NT_TERM_PRIME: return "term'";     break;
        case NT_FACTOR:     return "factor";    break;
        case NT_CONSTANT:   return "number";    break;
        case NT_IDENTIFIER: return "id";        break;
        case NT_ADD_OP:     return "+";         break;
        case NT_SUB_OP:     return "-";         break;
        case NT_MUL_OP:     return "*";         break;
        case NT_DIV_OP:     return "/";         break;
        default:
            break;
    }
    return "unknown node type";
}

NodeID NewNode(enum NodeType type)
{
    nodetab[nodetabIdx].type = type;
    nodetab[nodetabIdx].son = 0;
    nodetab[nodetabIdx].bro = 0;
    //printf("added node type %s\n", Type2Name(type));
    return nodetabIdx++;
}

NodeID AddSon(NodeID parent, NodeID node)
{
    NodeID next, last;
    
    next = SON(parent);
    if (next == 0)
    {
        SON(parent) = node;
        //printf("added node %s\n", Type2Name(TYPE(node)));
        return node;
    }
    while (next != 0)
    {
        last = next;
        next = BRO(next);
    }
    BRO(last) = node;
    
    //printf("added node %s\n", Type2Name(TYPE(node)));
    return node;
}

/*
    BASIC grammar:
    
    instr       : print | assignment
    print       : PRINT expr-list
    expr-list   : expr ','  expr-list | expr
    assignment  : [let] Identifier '=' expr
    expr        : term expr-prime
    expr-prime  : '+' term expr-prime | '-' term expr-prime | $
    term        : factor term-prime
    term-prime  : '*' factor term-prime | '/' factor term-prime | $
    factor  : '(' expr ')' | Constant | Identifier
*/

// instr : 
bool IsInstr(NodeID *pNode)
{
    NodeID son;
    
    *pNode = NewNode(NT_INSTR);
    
    // print
    if (IsPrint(&son))
    {
        AddSon(*pNode, son);
        return true;
    }
    
    // assignment
    else if (IsAssign(&son))
    {
        AddSon(*pNode, son);
        return true;
    }
    
    return false;
}

// print : PRINT expr-list
bool IsPrint(NodeID *pNode)
{
    NodeID son;
    
    if (token == PRINT)
    {
        *pNode = NewNode(NT_PRINT);        
        GetNextToken(NULL);
        if (IsExprList(&son))
        {
            AddSon(*pNode, son);
            return true;
        }
    }
    
    return false;
}
    
// expr-list :
bool IsExprList(NodeID *pNode)
{
    NodeID son;
    
    //  expr ','  expr-list | expr
    if (IsExpr(&son))
    {
        *pNode = NewNode(NT_EXPR_LIST);
        AddSon(*pNode, son);
        if (token == ',')
        {
            GetNextToken(NULL);
            if (IsExprList(&son))
            {
                AddSon(*pNode, son);
            }
            return true;
        }
        return true;
    }
    
    return false;
}
    
// assignment :
bool IsAssign(NodeID *pNode)
{
    NodeID son;
    
    // [let]
    if (token == LET)
    {
        // optional syntactic sugar
        GetNextToken(NULL);
    }
    
    // Identifier '=' expr
    if (token == Identifier)
    {
        *pNode = NewNode(NT_ASSIGN);
        son = NewNode(NT_IDENTIFIER);
        nodetab[son].symbol = lexsym;
        AddSon(*pNode, son);
        GetNextToken(NULL);
        if (token == '=')
        {
            GetNextToken(NULL);
            if (IsExpr(&son))
            {
                AddSon(*pNode, son);
                return true;
            }
        }
    }
    
    return false;
}

// expr :
bool IsExpr(NodeID *pNode)
{
    NodeID son;
    
    *pNode = NewNode(NT_EXPR);
    
    // term expr-prime
    if (IsTerm(&son))
    {
        AddSon(*pNode, son);
        if (IsExprPrime(&son))
        {
            AddSon(*pNode, son);
            return true;
        }
    }
    
    return false;
}

// expr-prime :
bool IsExprPrime(NodeID *pNode)
{
    NodeID son;
    
    // '+' term expr-prime
    if (token == '+')
    {
        *pNode = NewNode(NT_EXPR_PRIME);
        AddSon(*pNode, NewNode(NT_ADD_OP));
        GetNextToken(NULL);
        if (IsTerm(&son))
        {
            AddSon(*pNode, son);
            if (IsExprPrime(&son))
            {
                AddSon(*pNode, son);
                return true;
            }
            else
                return false;
        }
    }
    
    // '-' term expr-prime
    else if (token == '-')
    {
        *pNode = NewNode(NT_EXPR_PRIME);
        AddSon(*pNode, NewNode(NT_SUB_OP));
        GetNextToken(NULL);
        if (IsTerm(&son))
        {
            AddSon(*pNode, son);
            if (IsExprPrime(&son))
            {
                AddSon(*pNode, son);
                return true;
            }
            else
                return false;
        }
    }
    
    // $
    *pNode = 0;
    return true;
}

// term :
bool IsTerm(NodeID *pNode)
{
    NodeID son;
    
    // factor term-prime
    *pNode = NewNode(NT_TERM);    
    if (IsFactor(&son))
    {
        AddSon(*pNode, son);
        if (IsTermPrime(&son))
        {
            AddSon(*pNode, son);
            return true;
        }
    }

    return false;
}

// term-prime :
bool IsTermPrime(NodeID *pNode)
{
    NodeID son;
    
    // '*' factor term-prime
    if (token == '*')
    {
        *pNode = NewNode(NT_TERM_PRIME);
        AddSon(*pNode, NewNode(NT_MUL_OP));
        GetNextToken(NULL);
        if (IsFactor(&son))
        {
            AddSon(*pNode, son);
            if (IsTermPrime(&son))
            {
                AddSon(*pNode, son);
                return true;
            }
            else
                return false;
        }
    }
    
    // '/' factor term-prime
    else if (token == '/')
    {
        *pNode = NewNode(NT_TERM_PRIME);
        AddSon(*pNode, NewNode(NT_DIV_OP));
        GetNextToken(NULL);
        if (IsFactor(&son))
        {
            AddSon(*pNode, son);
            if (IsTermPrime(&son))
            {
                AddSon(*pNode, son);
                return true;
            }
            else
                return false;
        }
    }

    // $
    *pNode = 0;
    return true;
}

// factor :
bool IsFactor(NodeID *pNode)
{
    NodeID son;

    // '(' expr ')'
    *pNode = NewNode(NT_FACTOR);    
    if (token == '(')
    {
        GetNextToken(NULL);
        if (IsExpr(&son))
        {
            AddSon(*pNode, son);
            if (token == ')')
            {
                GetNextToken(NULL);
                return true;
            }
        }
    }
    
    // Constant
    else if (token == Constant)
    {
        //printf("%d F -> Constant\n", token);
        son = NewNode(NT_CONSTANT);
        CONSTANT(son) = atoi(lexeme);
        AddSon(*pNode, son);
        GetNextToken(NULL);
        return true;
    }
    
    // Identifier
    else if (token == Identifier)
    {
        //printf("%d F -> Identifier\n", token);
        son = NewNode(NT_IDENTIFIER);
        SYMBOL(son) = lexsym;
        AddSon(*pNode, son);
        GetNextToken(NULL);
        return true;
    }
    
    return false;
}

// tree traversal is guided by the grammar rule
bool TraverseTree(NodeID node)
{
    bool retval = true;
    //char tmpStr[80];
    
    if (node != 0)
    {
        //printf("node type %s\n", Type2Name(TYPE(node)));
        switch (TYPE(node))
        {
            case NT_INSTR:
                retval &= TraverseTree(SON(node));
                break;
                
            case NT_PRINT:
                // print
                //   expr-list
                retval &= TraverseTree(SON(node));
                break;
                
            case NT_EXPR_LIST:
            {
                char exprStr[80];
                // expr-list
                //   expr expr-list | expr
                retval &= TraverseTree(SON(node));          // expr and push expr value
                sprintf(exprStr, "%d", Pop());              // convert expr value to ascii
                strcat(resultStr, exprStr);                 // cat to existing result string
                if (BRO(SON(node)))
                {
                    strcat(resultStr, "\t");                // cat intervening tab
                    retval &= TraverseTree(BRO(SON(node))); // expr-list
                }
            }
                break;
                
            case NT_ASSIGN:
                // assignment
                //   Identifier expr
                retval &= TraverseTree(BRO(SON(node))); // eval
                SYMVAL(SYMBOL(SON(node))) = Pop();      // Identifier's symbol value = expr value
                SYMDEF(SYMBOL(SON(node))) = true;       // flag symbol as defined
                break;
                
            case NT_EXPR:
                // expr 
                //   term expr-prime
            case NT_TERM:
                // term
                //   factor term-prime
                retval &= TraverseTree(SON(node));
                retval &= TraverseTree(BRO(SON(node)));            
                break;
                
            case NT_EXPR_PRIME:
                // expr-prime
                //   '+' term expr-prime | '-' term expr-prime
            case NT_TERM_PRIME:
                // term-prime
                //   '*' factor term-prime | '/' factor term-prime
                retval &= TraverseTree(BRO(SON(node)));    // left opnd
                retval &= TraverseTree(BRO(BRO(SON(node))));    // right opnd
                retval &= TraverseTree(SON(node));   // op
                break;
                
            case NT_FACTOR:
                // factor
                //   expr | Constant | Identifier
                retval &= TraverseTree(SON(node));
                break;
                
            case NT_ADD_OP:
                Add();
                break;
                
            case NT_SUB_OP:
                Subtract();
                break;
                
            case NT_MUL_OP:
                Multiply();
                break;
                
            case NT_DIV_OP:
                Divide();
                break;
                
            case NT_CONSTANT:
                Push(CONSTANT(node));
                break;
                
            case NT_IDENTIFIER:
                if (SYMDEF(SYMBOL(node)) == true)
                {
                    Push(SYMVAL(SYMBOL(node)));
                }
                else
                {
                    sprintf(errorStr, "undefined variable %s", SYMNAME(SYMBOL(node)));
                    retval = false;
                }
                break;

            default:
                puts("unknown node type");
                retval = false;
                break;
        }
    }
    
    return retval;
}


bool ProcessCommand(char *command)
{
    NodeID root;
    
    // init the parser
    InitEvalStack();
    nodetabIdx = 1;
    errorStr[0] = '\0';
    resultStr[0] = '\0';
    
    // create the parse tree
    //nextChar = exprStr;
    GetNextToken(command);
    if (!IsInstr(&root))
    {
        strcpy(errorStr, "syntax error");
        return false;
    }
    
    // execute the instruction
    if (!TraverseTree(root))
    {
        return false;
    }
    
    return true;
}

// end of parser.c

