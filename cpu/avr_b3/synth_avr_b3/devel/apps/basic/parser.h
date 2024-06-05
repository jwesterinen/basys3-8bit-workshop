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
typedef int NodeID;
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

void InitParser(void);
bool GetExprValue(int *pValue);

