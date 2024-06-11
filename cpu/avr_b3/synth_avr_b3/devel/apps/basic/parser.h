/*
*   parser.h
*
*/

enum NodeType {
    NT_NONE = 0,
    NT_EXPR, NT_EXPR_PRIME, NT_TERM, NT_TERM_PRIME, NT_FACTOR, 
    NT_OP,
    NT_CONSTANT, NT_IDENTIFIER
};
union NodeValue {
    int constant;
    Symbol *symbol;
    char op;
};
typedef struct Node {
    enum NodeType type;
    union NodeValue value;
    struct Node *bro;
    struct Node *son;
} Node;

#define NODE_TYPE(node)         node->type
#define NODE_VAL_CONST(node)    node->value.constant
#define NODE_VAL_SYMBOL(node)   node->value.symbol
#define NODE_VAL_OP(node)       node->value.op
#define BRO(node)               node->bro
#define SON(node)               node->son

void InitParser(void);
bool GetExprValue(int *pValue);

