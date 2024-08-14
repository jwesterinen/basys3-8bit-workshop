/*
*   parser.c
*
*   An execution engine for a Basic language interpreter.
*
*   This module accepts a parsed internal representation of a Basic Language
*   command and executes the command.
*
*   Todo items:
*    - implement line removal, i.e. line number with no command removes that line in the program (replace with NOP??)
*/

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h>
#include "symtab.h"
#include "lexer.h"
#include "parser.h"
#include "main.h"

#define STRING_LEN 80
#define TABLE_LEN 100
#define STACK_SIZE 20
#define MAX_PROGRAM_LEN 100

bool RunProgram(void);
bool ListProgram(void);
bool ExecCommand(Command *command);
bool ExecPrint(PrintCommand *cmd);
bool ExecAssign(AssignCommand *cmd);
bool ExecFor(ForCommand *cmd);
bool ExecNext(NextCommand *cmd);
bool ExecGoto(GotoCommand *cmd);
bool ExecIf(IfCommand *cmd);
bool ExecGosub(GosubCommand *cmd);
bool ExecReturn(void);
bool ExecEnd(void);
bool ExecInput(InputCommand *cmd);
bool ExecPoke(PokeCommand *cmd);
bool ExecDim(DimCommand *cmd);
bool Peek(uint16_t addr);
bool ExecBuiltinFct(const char *name);
bool EvaluateNumExpr(Node *exprTreeRoot, float *pValue);
bool EvaluateStrExpr(Node *exprTreeRoot, char **pValue);
bool TraverseTree(Node *node);
float NumStackPush(float a);
float NumStackPop(void);
float NumStackTop(void);
float NumStackPut(float a);
char *StrStackPush(char *a);
char *StrStackPop(void);
char *StrStackTop(void);
char *StrStackPut(char *a);

// runtime strings and flags
bool ready = true;
char errorStr[STRING_LEN];
char resultStr[STRING_LEN];
int nodeCount = 0;


// ***stacks and queues***

// command queue aka "the program"
Command Program[MAX_PROGRAM_LEN];
int programIdx = 0;     // program index used to add commands to the program
int ip = 0;             // instruction pointer used to point to the current command in the program

// for command stack needed by the next command
ForCommand *fortab[TABLE_LEN];
int fortabIdx = 0;

// call stack used for subroutines/returns
int cs[STACK_SIZE];
unsigned csp = 0;

// floating point stack and its index, i.e. real stack pointer
float numStack[STACK_SIZE];
unsigned numSP = 0;

// string stack and its index, i.e. string stack pointer
char *strStack[STACK_SIZE];
unsigned strSP = 0;

// built-in function list
typedef struct FctList {
    char *name;
    int arity;
} FctList;

// convert a command line number to an index to the program
int LineNum2Ip(int lineNum)
{
    for (int i = 0; i < programIdx; i++)
        if (Program[i].lineNum == lineNum)
            return i;            
    return 0;
}

// convert an index to the program to a command line number
int Idx2lineNum(int idx)
{
    if (idx < programIdx)
        return Program[idx].lineNum;
    return 0;
}

void SwapProgLines(int progIdxA, int progIdxB) 
{ 
    // copy a to temp, b to a, then temp to b
    Command temp = Program[progIdxA]; 
    Program[progIdxA] = Program[progIdxB]; 
    Program[progIdxB] = temp; 
} 

void SortProgramByLineNum(void) 
{ 
    int i, j, min_idx; 
  
    // One by one move boundary of unsorted subarray 
    for (i = 0; i < programIdx - 1; i++) 
    { 
        // Find the minimum element in unsorted array 
        min_idx = i; 
        for (j = i + 1; j < programIdx; j++) 
            if (Program[j].lineNum < Program[min_idx].lineNum) 
                min_idx = j; 
  
        // Swap the found minimum element with the first element 
        SwapProgLines(min_idx, i); 
    } 
} 

void PrintResult(void)
{
    if (resultStr[0] != '\0')
    {
        PutString(resultStr);
        PutString("\n");
        resultStr[0] = '\0';
    }
}

// command : directive | statement
bool ProcessCommand(char *commandStr)
{
    // init the parser and error sting
    Command command = {0};
    bool isImmediate;
    int lineIdx;
    char tempStr[80];
    
    // default parser error
    strcpy(errorStr, "syntax error");
    
    // execute any directive
    if (!strcmp(commandStr, ""))
    {
        return true;
    }
    else if (!strcmp(commandStr, "run"))
    {
        return RunProgram();
    }
    else if (!strcmp(commandStr, "list"))
    {
        return ListProgram();
    }
    else if (!strcmp(commandStr, "new"))
    {
        fortabIdx = 0;
        programIdx = 0;
        csp = 0;
        numSP = 0;
        strSP = 0;
        FreeExprTrees();
        FreeSymtab();
        sprintf(message, "node qty: %d\n", nodeCount);
        MESSAGE(message);
        return true;
    }
    else if (!strcmp(commandStr, "reboot"))
    {
        fortabIdx = 0;
        programIdx = 0;
        csp = 0;
        numSP = 0;
        strSP = 0;
        FreeExprTrees();
        FreeSymtab();
        InitDisplay();
        return true;
    }

    // init the lexer and parse the command to create the IR
    if (GetNextToken(commandStr))
    {
        if (IsCommand(&command, &isImmediate))
        {
            if (isImmediate)
            {
                // the command has no line number so execute it immediately
                return ExecCommand(&command);
            }
            else
            {
                // check to see if this is a replacement of an existing line by number
                for (lineIdx = 0; lineIdx < programIdx; lineIdx++)
                {
                    if (command.lineNum == Idx2lineNum(lineIdx))
                    {
                        break;
                    }
                }
                Program[lineIdx] = command;
                if (lineIdx == programIdx)
                {
                    Program[lineIdx].lineNum = command.lineNum;
                    if (programIdx < MAX_PROGRAM_LEN-1)
                    {
                        programIdx++;
                    }
                    else
                    {
                        sprintf(errorStr, "no more program space");
                        return false;
                    }
                }
                SortProgramByLineNum();
                ready = false;
                return true;
            }
        }
        else
        {
            sprintf(tempStr, ": %s", commandStr);
            strcat(errorStr, tempStr);
        }
    }
    
    return false;
}

// execute commands in the program based on the Instruction Pointer (IP)
// which will simply increment unless a next or goto command is executed
// which will explicitly change the IP
bool RunProgram(void)
{
    char tempStr[STRING_LEN];
    
    // default error string
    strcpy(errorStr, "execution error");
    
    // ready for more commands whether or not the program executes correctly
    ready = true;    
      
    ip = 0;
    while (ip < programIdx)
    {
        // execute the command
        if (!ExecCommand(&Program[ip]))
        {
            //strcpy(errorStr, "execution error");
            sprintf(tempStr, " at line %d", Program[ip].lineNum);
            strcat(errorStr, tempStr);
            return false;
        }
    }
          
    return true;
}

bool ListProgram(void)
{
    for (int i = 0; i < programIdx; i++)
    {
        strcpy(resultStr, Program[i].commandStr);
        PrintResult();
    }
    ready = true;      
      
    return true;
}

bool ExecCommand(Command *command)
{
    switch (command->type)
    {
        case CT_PRINT: 
            if (!ExecPrint(&command->cmd.printCmd))
                return false;
            break;
                
        case CT_ASSIGN: 
            if (!ExecAssign(&command->cmd.assignCmd))
                return false;
            break;
                
        case CT_FOR: 
            if (!ExecFor(&command->cmd.forCmd))
                return false;
            break;
                
        case CT_NEXT: 
            if (!ExecNext(&command->cmd.nextCmd))
                return false;
            break;
                
        case CT_GOTO: 
            if (!ExecGoto(&command->cmd.gotoCmd))
                return false;
            break;                
                
        case CT_IF: 
            if (!ExecIf(&command->cmd.ifCmd))
                return false;
            break;                
                
        case CT_GOSUB: 
            if (!ExecGosub(&command->cmd.gosubCmd))
                return false;
            break;                
                
        case CT_RETURN: 
            if (!ExecReturn())
                return false;
            break;                
                
        case CT_END: 
            if (!ExecEnd())
                return false;
            break;                
                
        case CT_INPUT: 
            if (!ExecInput(&command->cmd.inputCmd))
                return false;
            break;                
                
        case CT_POKE: 
            if (!ExecPoke(&command->cmd.pokeCmd))
                return false;
            break;                
                
        case CT_DIM: 
            if (!ExecDim(&command->cmd.dimCmd))
                return false;
            break;                
    }    
    PrintResult();
    
    return true;
}

Node *GetPrimaryExprNode(Node *node)
{
    if (node == NULL || NODE_TYPE(node) == NT_PRIMARY_EXPR)
    {
        return node;
    }
    if (NODE_TYPE(node) == NT_UNOP)
    {
        return GetPrimaryExprNode(BRO(node));
    } 
    return GetPrimaryExprNode(SON(node));
}

// expr-list : expr  [{';' | ','} expr-list]
bool ExecPrint(PrintCommand *cmd)
{
    char exprStr[80];
    float numval;
    char *strval;
    int intval, decval;
    Node *primaryExprNode;
    
    for (int i = 0; i < cmd->printListIdx; i++)
    {
        strcat(resultStr, "");
        
        if (cmd->printList[i].separator == ',')
        {
            strcat(resultStr, "    ");
        }
        // TODO: the creation of a syntax tree would obviate this
        if ((primaryExprNode = GetPrimaryExprNode(cmd->printList[i].expr)) != NULL)
        {
            switch (NODE_TYPE(primaryExprNode->son))
            {
                case NT_NUMVAR:
                case NT_FCT:
                case NT_CONSTANT:
                    if (!EvaluateNumExpr(cmd->printList[i].expr, &numval))
                    {
                        strcpy(errorStr, "invalid print expression");
                        return false;
                    }
                    sprintf(exprStr, "%f", numval);
                    sscanf(exprStr, "%d.%d", &intval, &decval);
                    if (decval == 0)
                    {
                        sprintf(exprStr, "%.f", numval);
                    }
                    if (exprStr == NULL)
                    {
                        strcpy(errorStr, "invalid print expression");
                        return false;
                    }
                    strcat(resultStr, exprStr);
                    break;
                    
                case NT_STRVAR:
                case NT_STRING:
                    if (!EvaluateStrExpr(cmd->printList[i].expr, &strval))
                    {
                        strcpy(errorStr, "invalid print expression");
                        return false;
                    }
                    if (strval == NULL)
                    {
                        strcpy(errorStr, "invalid print expression");
                        return false;
                    }
                    strcat(resultStr, strval);
                    break;

                default:
                    break;
            }
        }
        else
        {
            return false;
        }
    }
    ip++;
    
    return true;
}

// assignment : {Intvar | Strvar} ['(' expr [',' expr]* ')'] '=' {expr | string}
// [let] Intvar ['(' expr [',' expr]* ')'] '=' expr
// [let] Strvar ['(' expr [',' expr]* ')'] '=' String | postfixExpr
bool ExecAssign(AssignCommand *cmd)
{
    float indeces[4] = {0};
    float numRhs;
    char *strRhs;
    
    // evaluate the LHS index values from their nodes if any
    for (int i = 0; i < SYM_DIM(cmd->varsym); i++)
    {
        if (!EvaluateNumExpr(cmd->indexNodes[i], &indeces[i]))
        {
            strcpy(errorStr, "invalid array index expression");
            return false;
        }
    }
            
    // perform the assignment
    if (SYM_TYPE(cmd->varsym) == ST_NUMVAR)
    {
        // RHS is an expression that must be evaluated
        if (EvaluateNumExpr(cmd->expr, &numRhs))
        {
            if (!SymWriteNumvar(cmd->varsym, indeces, numRhs))
            {
                return false;
            }
        }
    }        
    else if (SYM_TYPE(cmd->varsym) == ST_STRVAR)
    {
        // RHS is a string expr (e.g. array ref) that must be evaluated
        if (EvaluateStrExpr(cmd->expr, &strRhs))
        {
            if (!SymWriteStrvar(cmd->varsym, indeces, strRhs))
            {
                return false;
            }
        }
    }
    ip++;

    return true;
}

// for : FOR Intvar '=' init TO to [STEP step]
bool ExecFor(ForCommand *cmd)
{
    float value;
    
    //Console("ExecFor()\n");
    
    // perform initial variable assignment then push for command onto FOR stack
    if (EvaluateNumExpr(cmd->init, &value))
    {
        //SYM_NUMVAL(cmd->symbol) = value;
        if (SymWriteNumvar(cmd->symbol, NULL, value))
        {
            fortab[fortabIdx++] = cmd;
            ip++;
            return true;
        }
    }
    
    return false;
}

// next : NEXT [Intvar]
bool ExecNext(NextCommand *cmd)
{
    ForCommand *forInstr = NULL;
    float symval, to, step = 1;
    
    //Console("ExecNext()\n");
    
    // associate the next instruction with its matching for command
    if (cmd->symbol)
    {
        // find the explicit for instruction associated with the next instruction's var name
        for (int i = 0; i < fortabIdx; i++)
        {
            if (fortab[i]->symbol == cmd->symbol)
            {
                forInstr = fortab[i];
            }
        }
    }
    else
    {
        // use the for instruction on the top of the for stack for annonymous next
        forInstr = fortab[fortabIdx - 1];
    }
        
    // modify the associated variable and perform a goto if needed        
    if (forInstr->step)
    {
        if (!EvaluateNumExpr(forInstr->step, &step))
        {
            return false;
        }
    }
    //SYM_NUMVAL(forInstr->symbol) += step;
    if (SymReadNumvar(forInstr->symbol, NULL, &symval))
    {
        symval += step;
        if (SymWriteNumvar(forInstr->symbol, NULL, symval))
        {
            // check that the variable's value is in the range of the for instruction
            if (EvaluateNumExpr(forInstr->to, &to))
            {
                if (symval <= to)
                {
                    // goto command at line number following the for command
                    ip = LineNum2Ip(forInstr->lineNum) + 1;
                }
                else
                {
                    // continue program execution with the following command
                    ip++;
                }
                return true;
            }
        }
    }
    
    return false;
}

// goto : GOTO Constant
bool ExecGoto(GotoCommand *cmd)
{
    float dest;
    
    if (EvaluateNumExpr(cmd->dest, &dest))
    {
        if (dest > 0)
        {
            ip = LineNum2Ip(dest);
            return true;
        }
    }

    return false;
}

// if : IF expr THEN [assign | print | goto]
bool ExecIf(IfCommand *cmd)
{
    float predicate;
    
    if (EvaluateNumExpr(cmd->expr, &predicate))
    {
        // duh, but... if the predicate expr is true then perform 
        // the associated commands, else go on to the following command
        if (predicate)
        {
            switch (cmd->type)
            {
                case IT_PRINT:
                    return ExecPrint(&cmd->cmd.printCmd);
                case IT_ASSIGN:
                    return ExecAssign(&cmd->cmd.assignCmd);
                case IT_GOTO:
                    return ExecGoto(&cmd->cmd.gotoCmd);
                default:
                    strcpy(errorStr, "unknown sub-command in IF");
                    break;
            }
        }
        else
        {
            ip++;
            return true;
        }
    }

    return false;
}

// gosub : GOSUB Constant
bool ExecGosub(GosubCommand *cmd)
{
    float dest;
    
    //Console("ExecGosub()\n");
    
    if (EvaluateNumExpr(cmd->dest, &dest))
    {
        if (dest > 0)
        {
            // push return address, i.e. current IP + 1, onto call stack then go to the subroutine
            cs[csp++] = ip + 1;
            ip = LineNum2Ip(dest);
            return true;
        }
    }

    return false;
}

// return : RETURN
bool ExecReturn(void)
{
    
    //Console("ExecReturn()\n");
    
    // check for return without gosub error
    if (csp == 0)
    {
        strcpy(errorStr, "return without gosub");
        return false;
    }
    // pop the return address from the call stack and set the IP to it
    ip = cs[--csp];

    return true;
}

// end : END
bool ExecEnd(void)
{
    // end the program by setting IP to one past the last command in the program
    ip = programIdx;

    return true;
}

// TODO: change SymLookup to accept both a token value and an explicit token string, not the global "tokenStr"
// FIXME: a string input should be enclosed by quotes
// input : INPUT {Intvar | Strvar} ['(' expr [',' expr]* ')']
bool ExecInput(InputCommand *cmd)
{
    char buffer[80];
    float indeces[4];
    Node *expr;
    float numInput;
    char *strInput;
    
    // evaluate index values for an LHS array
    for (int i = 0; i < SYM_DIM(cmd->varsym); i++)
    {
        if (!EvaluateNumExpr(cmd->indexNodes[i], &indeces[i]))
        {
            strcpy(errorStr, "invalid array index expression");
            return false;
        }
    }
            
    //prompt with symbol name, space, question mark
    sprintf(buffer, "? ");
    PutString(buffer);
    
    // read and tokenize input text
    GetString(buffer);
    buffer[strlen(buffer)-1] = '\0';
    if (GetNextToken(buffer))
    {
        if (SYM_TYPE(cmd->varsym) == ST_NUMVAR)
        {
            // input can only be a constant
            if (token == Constant && IsExpr(&expr))
            {
                if (EvaluateNumExpr(expr, &numInput))
                {
                    if (!SymWriteNumvar(cmd->varsym, indeces, numInput))
                    {
                        return false;
                    }
                }
            }
            else
            {
                strcpy(errorStr, "invalid input expression");
                return false;
            }
        }        
        else if (SYM_TYPE(cmd->varsym) == ST_STRVAR)
        {
            // input can only be a string
            if (token == String && IsExpr(&expr))
            {
                if (EvaluateStrExpr(expr, &strInput))
                {
                    if (!SymWriteStrvar(cmd->varsym, indeces, strInput))
                    {
                        return false;
                    }
                }
            }
            else
            {
                strcpy(errorStr, "invalid input string");
                return false;
            }
        }
    }
    ip++;

    return true;
}

bool ExecPoke(PokeCommand *cmd)
{
    float numval;
    int addr, data;
    
    if (EvaluateNumExpr(cmd->addr, &numval))
    {
        addr = (int)numval;
        if (EvaluateNumExpr(cmd->data, &numval))
        {
            data = (int)numval;
            MemWrite((uint16_t)addr, (uint8_t)data);
            return true;
        }
    }
    
    return false;
}

// dim : DIM {Intvar | Strvar} '(' expr ')'
bool ExecDim(DimCommand *cmd)
{
    if (SYM_DIM(cmd->varsym) > 0)
    {
        if (EvaluateNumExpr(cmd->dimSizeNodes[0], &SYM_DIMSIZES(cmd->varsym, 0)))
        {
            SYM_SIZE(cmd->varsym) = SYM_DIMSIZES(cmd->varsym, 0);
        }
        else
        {
            strcpy(errorStr, "invalid dim expression");
            return false;
        }
    }
    // evaluate the dim sizes
    for (int i = 1; i < SYM_DIM(cmd->varsym); i++)
    {
        if (EvaluateNumExpr(cmd->dimSizeNodes[i], &SYM_DIMSIZES(cmd->varsym, i)))
        {
            SYM_SIZE(cmd->varsym) *= SYM_DIMSIZES(cmd->varsym, i);
        }
        else
        {
            strcpy(errorStr, "invalid dim expression");
            return false;
        }
    }

    /* TODO: might need dynamic allocation for multi-dim arrays    
    // allocate memory for the array
    switch (cmd->type)
    {
        case VT_EXPR:
            
            break;
        
        case VT_STRING:
            break;
        
        default:
            strcpy(errorStr, "unknown variable type");
            return false;
    }
    */    
    
    ip++;

    return true;
}

bool Peek(uint16_t addr)
{
    return NumStackPush(MemRead(addr));
}

bool ExecBuiltinFct(const char *name)
{
    if (!strcmp(name, "peek"))
    {
        return Peek((uint16_t)NumStackPop());
    } 
    
    return false;       
}

// return the value of an expression based on its expr tree
bool EvaluateNumExpr(Node *exprTreeRoot, float *pValue)
{
    int oldStrSP = strSP;
    
    // TODO: when TraverseTree returns the type of node found, it can direct which type of value to work with
    if (TraverseTree(exprTreeRoot))
    {
        *pValue = NumStackPop();
        return true;
    }
    
    strSP = oldStrSP;
    strcpy(errorStr, "incompatible type");
    return false;
}

// return the value of an expression based on its expr tree
bool EvaluateStrExpr(Node *exprTreeRoot, char **pValue)
{
    int oldIntSP = numSP;
    
    if (TraverseTree(exprTreeRoot))
    {
        *pValue = StrStackPop();
        return true;
    }
    
    numSP = oldIntSP;
    strcpy(errorStr, "incompatible type");
    return false;
}

// TODO: have this function return the last node type found
// tree traversal is guided by the grammar rule
bool TraverseTree(Node *node)
{
    bool retval = true;
    float indeces[4] = {0};
    int intval;
    float numval;
    char *strval;
    
    if (node != NULL)
    {
        switch (NODE_TYPE(node))
        {
            case NT_LOGIC_EXPR:
            case NT_REL_EXPR:
            case NT_SHIFT_EXPR:
            case NT_ADD_EXPR:
            case NT_MULT_EXPR:
                //  expr
	            //    : expr expr'
                retval &= TraverseTree(SON(node));
                retval &= TraverseTree(BRO(SON(node)));            
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
                retval &= TraverseTree(BRO(SON(node)));         // left opnd
                retval &= TraverseTree(BRO(BRO(SON(node))));    // right opnd
                retval &= TraverseTree(SON(node));              // binop
                break;
                
            case NT_UNARY_EXPR:
                // unaryExpr
                //   ['+' | '-' | NOT_OP] factor    
                retval &= TraverseTree(BRO(SON(node)));         // opnd
                retval &= TraverseTree(SON(node));              // unop
                break;

            case NT_POSTFIX_EXPR:
                // postfixExpr
                //   primaryExpr subExprList
                retval &= TraverseTree(BRO(SON(node)));         // push indeces or args, or nothing for intvar
                retval &= TraverseTree(SON(node));              // intvar, strvar, or fct
                break;
            
            case NT_SUB_EXPR_LIST:
                // subExprList
                //   addExpr ',' subExprList
                retval &= TraverseTree(SON(node));              // subscript
                retval &= TraverseTree(BRO(SON(node)));         // another subscript list
                break;
            
            case NT_PRIMARY_EXPR:
                // primaryExpr
                //   expr | Constant | Identifier
                retval &= TraverseTree(SON(node));
                break;
            
            case NT_BINOP:
                switch (NODE_VAL_OP(node))
                {    
                    case AND_OP:
                        // put the logical AND of the top 2 expr stack entries onto the top of the stack
                        NumStackPut(NumStackPop() && NumStackTop());
                        break;
                        
    
                    case OR_OP:
                        // put the logical OR of the top 2 expr stack entries onto the top of the stack
                        NumStackPut(NumStackPop() || NumStackTop());
                        break;
                        
    
                    case XOR_OP:
                        // put the exclusive OR of the top 2 expr stack entries onto the top of the stack
                        NumStackPut((float)((int)NumStackPop() ^ (int)NumStackTop()));
                        break;
                                                
                    case '=':
                        // put the equality relation of the top 2 expr stack entries onto the top of the stack
                        NumStackPut(NumStackPop() == NumStackTop());
                        break;                        
                        
                    case NE_OP:
                        // put the non-equality relation of the top 2 expr stack entries onto the top of the stack
                        NumStackPut(NumStackPop() != NumStackTop());
                        break;     
                                           
                    case '>':
                        // put the > relation of the top 2 expr stack entries onto the top of the stack
                        numval = NumStackPop();
                        NumStackPut(NumStackTop() > numval);
                        break;
                            
                    case GE_OP:
                        // put the >= relation of the top 2 expr stack entries onto the top of the stack
                        numval = NumStackPop();
                        NumStackPut(NumStackTop() >= numval);
                        break;
                        
                    case '<':
                        // put the < relation of the top 2 expr stack entries onto the top of the stack
                        numval = NumStackPop();
                        NumStackPut(NumStackTop() < numval);
                        break;
                        
    
                    case LE_OP:
                        // put the <= relation of the top 2 expr stack entries onto the top of the stack
                        numval = NumStackPop();
                        NumStackPut(NumStackTop() <= numval);
                        break;
                        
                    case '+':
                        // put the sum of the top 2 expr stack entries onto the top of the stack
                        NumStackPut(NumStackPop() + NumStackTop());
                        break;
                        
                    case '-':
                        // put the difference of the top 2 expr stack entries onto the top of the stack
                        numval = NumStackPop();
                        NumStackPut(NumStackTop() - numval);
                        break;
                        
                    case '*':
                        // put the product of the top 2 expr stack entries onto the top of the stack
                        NumStackPut(NumStackPop() * NumStackTop());
                        break;
                        
                    case '/':
                        // put the quotient of the top 2 expr stack entries onto the top of the stack
                        numval = NumStackPop();
                        NumStackPut(NumStackTop() / numval);
                        break;
                        
                    // NOTE: the following operators can only operate on
                    case '%':
                    case MOD_OP:
                        // put the modulus of the next to top of stack value by the top of stack value onto the top of the stack
                        intval = (int)NumStackPop();
                        NumStackPut((int)NumStackTop() % intval);
                        break;
                        
                    case SL_OP:
                        // put the left shift of the next to top of stack value by the top of stack onto the top of the stack
                        intval = (int)NumStackPop();
                        NumStackPut((int)NumStackTop() << intval);
                        break;
                        
                    case SR_OP:
                        // put the right shift of the next to top of stack value by the top of stack onto the top of the stack
                        intval = (int)NumStackPop();
                        NumStackPut((int)NumStackTop() >> intval);
                        break;
                }
                break;
                
            case NT_UNOP:
                switch (NODE_VAL_OP(node))
                {
                    case '+':
                        // do nothing to make something positive
                        break;
                        
                    case '-':
                        // negate the top of the expression
                        NumStackPut(-NumStackTop());
                        break;
                        
                    case NOT_OP:
                        // logically invert the top of the expression stack
                        NumStackPut(!NumStackTop());
                        break;
                }
                break;
                
            case NT_CONSTANT:
                NumStackPush(NODE_VAL_CONST(node));
                break;
                
            // note: for arrays, pop the indeces into an index array then read the value, 
            // the popped values will be the reverse of the needed indeces so reverse them in the indeces array
            case NT_NUMVAR:
                for (int i = 0; i < SYM_DIM(NODE_VAL_VARSYM(node)); i++)
                {
                    if ((intval = (int)NumStackPop()) == -1)
                    {
                        retval = false;
                        break;
                    }
                    indeces[(int)(SYM_DIM(NODE_VAL_VARSYM(node))) - i - 1] = intval;
                }
                if ((retval = SymReadNumvar(NODE_VAL_VARSYM(node), indeces, &numval)))
                {
                    NumStackPush(numval);
                }
                break;

            case NT_STRVAR:                
                for (int i = 0; i < SYM_DIM(NODE_VAL_VARSYM(node)); i++)
                {
                    if ((intval = (int)NumStackPop()) == -1)
                    {
                        retval = false;
                        break;
                    }
                    indeces[(int)(SYM_DIM(NODE_VAL_VARSYM(node))) - i - 1] = intval;
                }
                if ((retval = SymReadStrvar(NODE_VAL_VARSYM(node), indeces, &strval)))
                {
                    StrStackPush(strval);
                }
                break;
                

            case NT_FCT:
                // exec the builtin fct which will put the result on the stack
                ExecBuiltinFct(SYM_NAME(NODE_VAL_VARSYM((node))));
                break;

            case NT_STRING:
                // push the node's string value onto the string stack
                StrStackPush(NODE_VAL_STRING(node));
                break;
                
            default:
                puts("unknown node type");
                retval = false;
                break;
        }
    }
    
    return retval;
}

// number stack functions
float NumStackPush(float a)
{
    if (numSP < STACK_SIZE)
    {
        numStack[numSP++] = a;
        return a;
    }
    return -1;
}
float NumStackPop(void)
{
    if (numSP > 0)
    {
        return numStack[--numSP];
    }
    return -1;
} 
float NumStackTop(void)
{
    return numStack[numSP-1];
} 
float NumStackPut(float a)
{
    if (numSP > 0)
    {
        numStack[numSP-1] = a;
        return a;
    }
    return -1;
}

// string stack functions
char *StrStackPush(char *a)
{
    if (strSP < STACK_SIZE)
    {
        strStack[strSP++] = a;
        return a;
    }
    return NULL;
}
char *StrStackPop(void)
{
    if (strSP > 0)
    {
        return strStack[--strSP];
    }
    return "";
} 
char *StrStackTop(void)
{
    return strStack[strSP-1];
} 
char *StrStackPut(char *a)
{
    if (strSP > 0)
    {
        strStack[strSP-1] = a;
        return a;
    }
    return "";
}

// end of runtime.c

