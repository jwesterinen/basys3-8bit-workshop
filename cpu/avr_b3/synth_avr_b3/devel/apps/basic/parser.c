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
    NT_EXPR, NT_EXPR_PRIME, NT_TERM, NT_TERM_PRIME, NT_FACTOR, 
    NT_OP,
    NT_CONSTANT, NT_IDENTIFIER
};
typedef uint8_t NodeID;
union NodeValue {
    int constant;
    SymbolID symbol;
    char op;
};
typedef struct Node {
    enum NodeType type;
    union NodeValue value;
    NodeID bro;
    NodeID son;
} Node;

#define TYPE(node)      nodetab[(node)].type
#define CONSTANT(node)  nodetab[(node)].value.constant
#define SYMBOL(node)    nodetab[(node)].value.symbol
#define OP(node)        nodetab[(node)].value.op
#define BRO(node)       nodetab[(node)].bro
#define SON(node)       nodetab[(node)].son
#define SYMNAME(symbol) symtab[(symbol)].name
#define SYMVAL(symbol)  symtab[(symbol)].value

char *Type2Name(enum NodeType type);
NodeID NewNode(enum NodeType type, union NodeValue value);
NodeID AddSon(NodeID parent, NodeID node);
bool IsInstr(void);
bool IsPrint(void);
bool IsExprList(void);
bool IsAssign(void);
bool IsExpr(NodeID *pNode);
bool IsExprPrime(NodeID *pNode);
bool IsTerm(NodeID *pNode);
bool IsTermPrime(NodeID *pNode);
bool IsFactor(NodeID *pNode);
bool TraverseTree(NodeID node);

char errorStr[80];
char resultStr[80];

Node nodetab[100];
int nodetabIdx = 1;

char *Type2Name(enum NodeType type)
{
    switch (type)
    {
        case NT_EXPR:       return "expr";      break;
        case NT_EXPR_PRIME: return "expr'";     break;
        case NT_TERM:       return "term";      break;
        case NT_TERM_PRIME: return "term'";     break;
        case NT_FACTOR:     return "factor";    break;
        case NT_CONSTANT:   return "number";    break;
        case NT_IDENTIFIER: return "id";        break;
        default:
            break;
    }
    return "unknown node type";
}

NodeID NewNode(enum NodeType type, union NodeValue value)
{
    nodetab[nodetabIdx].type = type;
    nodetab[nodetabIdx].son = 0;
    nodetab[nodetabIdx].bro = 0;
    nodetab[nodetabIdx].value = value;
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
    
    instr           : print | assignment
    print           : PRINT print-list
    print-list      ; printable  [';' | ','] print-list | printable
    printable       : expr | String
    assignment      : [let] Identifier '=' expr
    str-assignment  : [let] Identifier '=' expr
    expr            : term expr-prime
    expr-prime      : ['+' | '-'] term expr-prime | $
    term            : factor term-prime
    term-prime      : ['*' | '/'] factor term-prime | $
    factor          : '(' expr ')' | Constant | Identifier
*/

// instr : print | assignment
bool IsInstr()
{
    // print
    if (IsPrint() || IsAssign())
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
        GetNextToken(NULL);
        if (IsExprList())
        {
            return true;
        }
    }
    
    return false;
}
    
// expr-list : expr [';' | ','] expr-list | expr
bool IsExprList()
{
    NodeID expr;
    char exprStr[80];
    
    if (IsExpr(&expr))
    {
        if (TraverseTree(expr))
        {
            sprintf(exprStr, "%d", Pop());              // convert expr value to ascii
            strcat(resultStr, exprStr);                 // cat to existing result string
            if (token == ';' || token == ',')
            {
                if (token == ',')
                {
                    strcat(resultStr, "\t");                // cat intervening tab
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
    }
    
    return false;
}
    
// assignment : [let] Identifier '=' expr
bool IsAssign()
{
    NodeID node;
    
    if (token == LET)
    {
        // optional syntactic sugar
        GetNextToken(NULL);
    }
    if (token == Identifier)
    {
        GetNextToken(NULL);
        if (token == '=')
        {
            GetNextToken(NULL);
            if (IsExpr(&node))
            {
                if (TraverseTree(node))
                {
                    SYMVAL(lexsym) = Pop();      // Identifier's symbol value = expr value
                    return true;
                }
            }
        }
    }
    
    return false;
}

// expr : term expr-prime
bool IsExpr(NodeID *pNode)
{
    NodeID son;
    
    *pNode = NewNode(NT_EXPR, (union NodeValue)0);
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

// expr-prime : ['+' | '-'] term expr-prime | $
bool IsExprPrime(NodeID *pNode)
{
    NodeID son;
    
    if (token == '+' || token == '-')
    {
        *pNode = NewNode(NT_EXPR_PRIME, (union NodeValue)0);
        AddSon(*pNode, NewNode(NT_OP, (union NodeValue)token));
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

// term : factor term-prime
bool IsTerm(NodeID *pNode)
{
    NodeID son;
    
    *pNode = NewNode(NT_TERM, (union NodeValue)0);    
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

// term-prime : '*' factor term-prime
bool IsTermPrime(NodeID *pNode)
{
    NodeID son;
    
    if (token == '*' || token == '/')
    {
        *pNode = NewNode(NT_TERM_PRIME, (union NodeValue)0);
        AddSon(*pNode, NewNode(NT_OP, (union NodeValue)token));
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
    *pNode = NewNode(NT_FACTOR, (union NodeValue)0);    
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
        AddSon(*pNode, NewNode(NT_CONSTANT, (union NodeValue)atoi(lexeme)));
        GetNextToken(NULL);
        return true;
    }
    
    // Identifier
    else if (token == Identifier)
    {
        //printf("%d F -> Identifier\n", token);
        AddSon(*pNode, NewNode(NT_IDENTIFIER, (union NodeValue)lexsym));
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
            
            case NT_OP:
                switch (OP(node))
                {
                    case '+':
                        Add();
                        break;
                        
                    case '-':
                        Subtract();
                        break;
                        
                    case '*':
                        Multiply();
                        break;
                        
                    case '/':
                        Divide();
                        break;
                }
                break;
                
            case NT_CONSTANT:
                Push(CONSTANT(node));
                break;
                
            case NT_IDENTIFIER:
                Push(SYMVAL(SYMBOL(node)));
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
    // init the parser
    InitEvalStack();
    nodetabIdx = 1;
    errorStr[0] = '\0';
    resultStr[0] = '\0';
    
    // parse the command
    GetNextToken(command);
    if (!IsInstr())
    {
        strcpy(errorStr, "syntax error");
        return false;
    }    
    
    return true;
}

// end of parser.c

