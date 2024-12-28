/*
 *  ir.h
 *
 *  This file contains the interface to the Basic Interpreter internal representation.
 *
 */

enum NodeType {
    NT_NONE = 0,
    NT_EXPR, 
    NT_LOGIC_EXPR, NT_LOGIC_EXPR_PRIME, 
    NT_REL_EXPR, NT_REL_EXPR_PRIME, 
    NT_SHIFT_EXPR, NT_SHIFT_EXPR_PRIME, 
    NT_ADD_EXPR, NT_ADD_EXPR_PRIME, 
    NT_MULT_EXPR, NT_MULT_EXPR_PRIME, 
    NT_UNARY_EXPR, 
    NT_POSTFIX_EXPR, NT_SUB_EXPR_LIST, 
    NT_PRIMARY_EXPR,
    
    // operator types
    NT_BINOP, NT_UNOP,
    
    // primary expression types
    NT_CONSTANT, NT_NUMVAR, NT_STRVAR, NT_FCT, NT_STRING
};

union NodeValue {
    Symbol *varsym;
    float constant;
    char *string;
    int op;
};
#define NODE_TYPE(node)         (node)->type
#define NODE_VAL_VARSYM(node)   (node)->value.varsym
#define NODE_VAL_CONST(node)    (node)->value.constant
#define NODE_VAL_STRING(node)   (node)->value.string
#define NODE_VAL_OP(node)       (node)->value.op
#define BRO(node)               (node)->bro
#define SON(node)               (node)->son

typedef struct PT_Node {
    enum NodeType type;
    union NodeValue value;
    struct PT_Node *bro;
    struct PT_Node *son;
} PT_Node;

void FreeExprTrees(void);
bool IsExpr(PT_Node **ppNode);
bool IsPostfixExpr(PT_Node **ppNode);
bool IsPrimaryExpr(PT_Node **ppNode);

extern int gNodeQty;

