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
#include "eval_stack.h"

enum NodeType {
    TYPE_NONE = 0,
    TYPE_INSTR,
    TYPE_PRINT, TYPE_ASSIGN,
    TYPE_E, TYPE_EP, TYPE_T, TYPE_TP, TYPE_F, 
    TYPE_PLUS_OP, TYPE_MULT_OP, TYPE_ASSIGN_OP, TYPE_PRINT_OP,
    TYPE_LPAREN, TYPE_RPAREN, 
    TYPE_NUM, TYPE_ID
};
typedef uint8_t NodeID;
typedef struct Node {
    NodeID id;
    enum NodeType type;
    union {
        int op;
        int constant;
        SymbolID symbol;
    } value;
    NodeID son;
    NodeID bro;
} Node;

char *Type2Name(enum NodeType type);
NodeID NewNode(enum NodeType type);
NodeID AddSon(NodeID parent, NodeID node);
bool IsInstr(NodeID *pNode);
bool IsPrint(NodeID *pNode);
bool IsAssign(NodeID *pNode);
bool IsExpr(NodeID *pNode);
bool IsExprPrime(NodeID *pNode);
bool IsTerm(NodeID *pNode);
bool IsTermPrime(NodeID *pNode);
bool IsFactor(NodeID *pNode);

char *nextChar;
int token;
char lexeme[80];
SymbolID lexsym;
int i = 0;
char errorStr[80];
extern int result;

Node nodetab[100];
int nodetabIdx = 1;

char *Type2Name(enum NodeType type)
{
    switch (type)
    {
        case TYPE_PLUS_OP:      return "+";         break;
        case TYPE_MULT_OP:      return "*";         break;
        case TYPE_ASSIGN_OP:    return "=";         break;
        case TYPE_LPAREN:       return "(";         break;
        case TYPE_RPAREN:       return ")";         break;
        case TYPE_NUM:          return "number";    break;
        case TYPE_ID:           return "id";        break;
        case TYPE_E:            return "<expr>";    break;
        case TYPE_EP:           return "<expr'>";   break;
        case TYPE_T:            return "<term>";    break;
        case TYPE_TP:           return "<term'>";   break;
        case TYPE_F:            return "<factor>";  break;
        case TYPE_INSTR:        return "<instr>";   break;
        case TYPE_PRINT:        return "<print>";  break;
        case TYPE_ASSIGN:       return "<assign>";  break;
        default:
            break;
    }
    return "unknown node type";
}

NodeID NewNode(enum NodeType type)
{
    nodetab[nodetabIdx].id = nodetabIdx;
    nodetab[nodetabIdx].type = type;
    nodetab[nodetabIdx].son = 0;
    nodetab[nodetabIdx].bro = 0;
    //printf("added node type %s\n", Type2Name(type));
    return nodetabIdx++;
}

NodeID AddSon(NodeID parent, NodeID node)
{
    NodeID next, last;
    
    next = nodetab[parent].son;
    if (next == 0)
    {
        nodetab[parent].son = node;
        //printf("added node %s\n", Type2Name(nodetab[node].type));
        return node;
    }
    while (next != 0)
    {
        last = next;
        next = nodetab[next].bro;
    }
    nodetab[last].bro = node;
    
    //printf("added node %s\n", Type2Name(nodetab[node].type));
    return node;
}

/*
*   lexical grammar
*
*   letter          [a-zA-Z_]
*   digit           [0-9]
*   letter_or_digit [a-zA-Z_0-9]
*   
*   "print"                     return PRINT
*   {letter}{letter_or_digit}*  return Identifier
*   {digit}+                    return Number
*   {other}                     return .
*/

// token values, single char tokens use their ASCII values
#define NUMBER  300
#define ID      301
#define PRINT   302
#define LET     303

struct KeywordTableEntry {
    char *keyword;
    int token;
} keywordTab[] = {
        {"print", PRINT},
        {"let", LET},
};
int keywordTableSize = sizeof keywordTab / sizeof(struct KeywordTableEntry);

// return the next token in the instruction
void GetNextToken(void)
{
    static int state = 0;
    
    //printf("\tGetNextToken: \n");
    
    while (1)
    {
        switch (state)
        {
            case 0:
                // remove whitespace
                //printf("\t\tstate 0, remove whitespace\n");
                if (isspace(*nextChar))
                {
                    nextChar++;
                }
                else
                {
                    state = 1;
                }
                break;
                
            case 1:
                // direct the lexer to parse a number, ID, or single char
                //printf("\t\tstate 1, direct lexer\n");
                if (isdigit(*nextChar))
                {
                    lexeme[i++] = *nextChar++;
                    state = 2;
                }
                else if (isalpha(*nextChar))
                {
                    lexeme[i++] = *nextChar++;
                    state = 3;
                }
                else if (*nextChar != '\0')
                {
                    lexeme[0] = *nextChar++;
                    state = 6;
                }
                else
                {
                    state = 7;
                }
                break;
                
            case 2:
                // parse a number
                //printf("\t\tstate 3, parse a number\n");
                if (isdigit(*nextChar))
                {
                    lexeme[i++] = *nextChar++;
                }
                else
                {
                    state = 4;
                }
                break;
                
            case 3:
                // parse an ID or keyword
                //printf("\t\tstate 4, parse in ID\n");
                if (isalnum(*nextChar))
                {
                    lexeme[i++] = *nextChar++;
                }
                else
                {
                    state = 5;
                }
                break;
                
            case 4:
                // return NUMBER token
                lexeme[i] = '\0';
                //printf("\t\tstate 5, token = NUMBER %s\n", lexeme);
                i = 0;
                state = 0;
                token = NUMBER;
                return;
                
            case 5:
                // return ID or keyword token
                lexeme[i] = '\0';
                //printf("\t\tstate 6, token = ID %s\n", lexeme);
                i = 0;
                state = 0;
                
                // if the lexeme is a keyword, return its corresponding token
                for (int j = 0; j < keywordTableSize; j++)
                {
                    if (!strcmp(keywordTab[j].keyword, lexeme))
                    {
                        token = keywordTab[j].token;
                        return;
                    }
                }
                
                // if the lexeme isn't a keyword, lookup a symbol and return ID
                token = ID;
                lexsym = SymLookup(lexeme);
                return;
                
            case 6:
                // return single char token
                lexeme[1] = '\0';
                //printf("\t\tstate 7, token = single char token %c\n", lexeme[0]);
                i = 0;
                state = 0;
                token = lexeme[0];
                return;
                
            case 7:
                // return EOL
                //printf("\t\tstate 8, token = EOL\n");
                i = 0;
                state = 0;
                token = 0;
                return;
        }
    }
}

/*
    BASIC grammar:
    
    <instr>         := <print> | <assignment> | <expr>
    <print>         := PRINT <expr>
    <assignment>    := [let] ID '=' <expr>
    <expr>          := <term> <expr-prime>
    <expr-prime>    := '+' <term> <expr-prime> | $
    <term>          := <factor> <term-prime>
    <term-prime>    := '*' <factor> <term-prime> | $
    <factor>        := '(' <expr> ')' | NUMBER | ID
*/

// <instr> := <assignment> | <print> | <expr>
bool IsInstr(NodeID *pNode)
{
    NodeID son;
    
    *pNode = NewNode(TYPE_INSTR);
    if (IsPrint(&son))
    {
        AddSon(*pNode, son);
        return true;
    }
    else if (IsAssign(&son))
    {
        AddSon(*pNode, son);
        return true;
    }
    
    return false;
}

// <print> := PRINT <expr>
bool IsPrint(NodeID *pNode)
{
    NodeID son;
    
    if (token == PRINT)
    {
        *pNode = NewNode(TYPE_PRINT);
        
        AddSon(*pNode, NewNode(TYPE_PRINT_OP));
        GetNextToken();
        if (IsExpr(&son))
        {
            AddSon(*pNode, son);
            return true;
        }
        else
        {
            return false;
        }
    }
    
    return false;
}
    
// <assignment> := ID '=' <expr>
bool IsAssign(NodeID *pNode)
{
    NodeID son;
    
    if (token == LET)
    {
        // optional syntactic sugar
        GetNextToken();
    }
    if (token == ID)
    {
        *pNode = NewNode(TYPE_ASSIGN);
        son = NewNode(TYPE_ID);
        nodetab[son].value.symbol = lexsym;
        AddSon(*pNode, son);
        GetNextToken();
        if (token == '=')
        {
            AddSon(*pNode, NewNode(TYPE_ASSIGN_OP));
            GetNextToken();
            if (IsExpr(&son))
            {
                AddSon(*pNode, son);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
            return false;
    }
    
    return false;
}

// <expr> := <term> <expr-prime>
bool IsExpr(NodeID *pNode)
{
    NodeID son;
    
    //printf("%d E -> T E'\n", token);
    *pNode = NewNode(TYPE_E);
    
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
    
    return false;
}

// <expr-prime> := '+' <term> <expr-prime> | $
bool IsExprPrime(NodeID *pNode)
{
    NodeID son;
    
    if (token == '+')
    {
        *pNode = NewNode(TYPE_EP);
        //printf("%d E' -> + T E'\n", token);
        AddSon(*pNode, NewNode(TYPE_PLUS_OP));
        GetNextToken();
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

    //printf("%d E' -> $\n", token);
    *pNode = 0;
    return true;
}

// <term> := <factor> <term-prime>
bool IsTerm(NodeID *pNode)
{
    NodeID son;
    
    //printf("%d T -> F T'\n", token);
    *pNode = NewNode(TYPE_T);    
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

    return false;
}

// <term-prime> := '*' <factor> <term-prime> | $
bool IsTermPrime(NodeID *pNode)
{
    NodeID son;
    
    if (token == '*')
    {
        *pNode = NewNode(TYPE_TP);
        //printf("%d T' -> * F T'\n", token);
        AddSon(*pNode, NewNode(TYPE_MULT_OP));
        GetNextToken();
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

    //printf("%d T' -> $\n", token);
    *pNode = 0;
    return true;
}

// <factor> := '(' <expr> ')' | NUMBER | ID
bool IsFactor(NodeID *pNode)
{
    NodeID son;

    *pNode = NewNode(TYPE_F);    
    if (token == '(')
    {
        //printf("%d F -> ( E )\n", token);
        AddSon(*pNode, NewNode(TYPE_LPAREN));
        GetNextToken();
        if (IsExpr(&son))
        {
            AddSon(*pNode, son);
            if (token == ')')
            {
                AddSon(*pNode, NewNode(TYPE_RPAREN));
                GetNextToken();
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else if (token == NUMBER)
    {
        //printf("%d F -> NUMBER\n", token);
        son = NewNode(TYPE_NUM);
        nodetab[son].value.constant = atoi(lexeme);
        AddSon(*pNode, son);
        GetNextToken();
        return true;
    }
    else if (token == ID)
    {
        //printf("%d F -> ID\n", token);
        son = NewNode(TYPE_ID);
        nodetab[son].value.symbol = lexsym;
        AddSon(*pNode, son);
        GetNextToken();
        return true;
    }
    
    return false;
}

// depth first traversal
bool TraverseTree(NodeID node)
{
    NodeID son;
    SymbolID symbol;
    bool retval = true;
    
    if (node != 0)
    {
        //printf("node type %s\n", Type2Name(nodetab[node].type));
        switch (nodetab[node].type)
        {
            // leaf nodes
            case TYPE_PLUS_OP:
                Add();
                break;
            case TYPE_MULT_OP:
                Multiply();
                break;
            case TYPE_NUM:
                Push(nodetab[node].value.constant);
                break;
            case TYPE_ID:
                symbol = nodetab[node].value.symbol;
                if (symtab[symbol].isDefined)
                {
                    Push(symtab[symbol].value);
                }
                else
                {
                    sprintf(errorStr, "undefined variable %s", symtab[symbol].name);
                    retval = false;
                }
                break;

            // internal nodes
            case TYPE_INSTR:
                node = nodetab[node].son;
                retval &= TraverseTree(node);
                break;
            case TYPE_PRINT:
                // <print> := PRINT <expr>
                son = nodetab[node].son;        // PRINT
                son = nodetab[son].bro;         // <expr>
                retval &= TraverseTree(son);
                break;
            case TYPE_ASSIGN:
                // <assignment> := ID '=' <expr>
                son = nodetab[node].son;            // ID
                son = nodetab[son].bro;             // '='
                son = nodetab[son].bro;             // <expr>
                retval &= TraverseTree(son);        // push expr value
                son = nodetab[node].son;            // ID
                symbol = nodetab[son].value.symbol; // ID's symbol
                symtab[symbol].value = Top();       // ID's symbol value defined as expr value
                symtab[symbol].isDefined = true;
                break;
            case TYPE_E:
            case TYPE_T:
                node = nodetab[node].son;
                retval &= TraverseTree(node);
                node = nodetab[node].bro;
                retval &= TraverseTree(node);            
                break;
            case TYPE_EP:
            case TYPE_TP:
                node = nodetab[node].son;
                son = nodetab[node].bro;
                retval &= TraverseTree(son);
                son = nodetab[son].bro;
                retval &= TraverseTree(son);
                retval &= TraverseTree(node);
                break;
            case TYPE_F:
                node = nodetab[node].son;
                if (nodetab[node].type == TYPE_LPAREN)
                    node = nodetab[node].bro;
                retval &= TraverseTree(node);
                break;
                
            default:
                puts("unknown node type");
                retval = false;
                break;
        }
    }
    
    return retval;
}


bool Parse(char *exprStr)
{
    NodeID root;
    
    // init the parser
    InitEvalStack();
    nodetabIdx = 1;
    
    // create the parse tree
    nextChar = exprStr;
    GetNextToken();
    if (!IsInstr(&root))
    {
        strcpy(errorStr, "syntax error");
        return false;
    }
    
    // evaluate the expression
    if (!TraverseTree(root))
    {
        return false;
    }
    result = Top();
    
    return true;
}

// end of parser.c

