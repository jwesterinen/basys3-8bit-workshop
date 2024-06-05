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
#include "symtab.h"
#include "lexer.h"
#include "parser.h"

#define STACK_SIZE 100

// eval stack and its index, i.e. eval stack pointer
int es[STACK_SIZE];
unsigned esp;
int Push(int a)
{
    if (esp < STACK_SIZE)
    {
        es[esp++] = a;
        return a;
    }
    return -1;
}
int Pop(void)
{
    return es[--esp];
} 
int Top(void)
{
    return es[esp-1];
} 
int Put(int a)
{
    es[esp-1] = a;
    return a;
}

char *Type2Name(enum NodeType type);
NodeID NewNode(enum NodeType type, union NodeValue value);
NodeID AddSon(NodeID parent, NodeID node);
bool IsExpr(NodeID *pNode);
bool IsExprPrime(NodeID *pNode);
bool IsTerm(NodeID *pNode);
bool IsTermPrime(NodeID *pNode);
bool IsFactor(NodeID *pNode);
bool TraverseTree(NodeID node);

// node table and index
#define NODETAB_LEN 100
Node nodetab[NODETAB_LEN];
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

void InitParser()
{
    // init eval stack pointer and node table
    esp = 0;
    nodetabIdx = 1;
}

// return the value of an expression
bool GetExprValue(int *pValue)
{
    NodeID node;
    
    // create an expression tree and traverse it to produce the value on the eval stack
    if (IsExpr(&node))
    {
        if (TraverseTree(node))
        {
            *pValue = Pop();
            return true;
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
                        // put the sum of the top 2 expr stack entries onto the top of the stack and set the new value
                        Put(Pop() + Top());
                        break;
                        
                    case '-':
                    {
                        // put the difference of the top 2 expr stack entries onto the top of the stack and set the new value
                        int b = Pop();
                        Put(Top() - b);
                    }
                        break;
                        
                    case '*':
                        // put the sum of the top 2 expr stack entries onto the top of the stack and set the new value
                        Put(Pop() * Top());
                        break;
                        
                    case '/':
                    {
                        // put the difference of the top 2 expr stack entries onto the top of the stack and set the new value
                        int b = Pop();
                        Put(Top() / b);
                    }
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

// end of parser.c

