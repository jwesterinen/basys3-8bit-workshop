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
#include <math.h>
#include "symtab.h"
#include "lexer.h"
#include "parser.h"
#include "main.h"

#define STRING_LEN 80
#define TABLE_LEN 100
#define STACK_SIZE 20
#define MAX_PROGRAM_LEN 100

#define BEEP_TONE 440
#define BEEP_DURATION 300

#define ALL_COMMANDS false

bool RunProgram(void);
bool ListProgram(void);
bool ExecCommand(Command *command, bool cmdLineOnly);
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
bool ExecPoke(PlatformCommand *cmd);
bool ExecTone(PlatformCommand *cmd);
bool ExecBeep(PlatformCommand *cmd);
bool ExecDisplay(PlatformCommand *cmd);
bool ExecOutchar(PlatformCommand *cmd);
bool ExecRseed(PlatformCommand *cmd);
bool ExecDelay(PlatformCommand *cmd);
bool ExecDim(DimCommand *cmd);

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
CommandLine Program[MAX_PROGRAM_LEN]; // list of command lines all of which share the same line number
int programIdx = 0;     // program index used to add commands to the program
int cmdLineIdx = 0;     // command line index used to point to the current command line in the program
Command *cmdPtr = NULL; // command pointer is used to point to the next command to be executed

// for command stack needed by the next command
ForCommand *fortab[TABLE_LEN];
int fortabIdx = 0;

// call stack used for subroutines/returns
Command *callStack[STACK_SIZE];
unsigned callSP = 0;

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

// free a command list
void FreeCommand(Command *cmd)
{
    if (cmd->next)
        FreeCommand(cmd->next);
    free(cmd);
}

// free all allocated commands in a command line
void FreeCommandLine(CommandLine *cmdLine)
{
    if (cmdLine->cmd.next)
        FreeCommand(cmdLine->cmd.next);
    cmdLine->cmd.next = NULL;
}

// free all allocated commands in a command line
void FreeProgram(void)
{
    for (int i = 0; i < programIdx; i++)
    {
        FreeCommandLine(&Program[i]);
    }
}

Command *IterateCmdPtr(bool cmdLineOnly)
{
    if (cmdPtr->next)
    {
        // next command in the command line
        return cmdPtr->next;
    }
    else if ((!cmdLineOnly) && (cmdLineIdx < programIdx))
    {
        // first command in the next command line
        return &Program[++cmdLineIdx].cmd;
    }
    
    return NULL;
}

// convert a line number to a program index
bool LineNum2CmdLineIdx(int lineNum)
{
    for (int i = 0; i < programIdx; i++)
    {
        if (Program[i].lineNum == lineNum)
        {
            cmdLineIdx = i;
            return true;
        }
    }
    
    return false;
}

void SwapProgLines(int progIdxA, int progIdxB) 
{ 
    // copy a to temp, b to a, then temp to b
    CommandLine temp = Program[progIdxA]; 
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

// this is the entry point from the main UI into the interpreter
// command : directive | command-line
bool ProcessCommand(char *commandStr)
{
    // init the parser and error string
    CommandLine commandLine = {0};
    bool isImmediate;
    int i;
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
        cmdLineIdx = 0;
        callSP = 0;
        numSP = 0;
        strSP = 0;
        FreeExprTrees();
        FreeSymtab();
        FreeProgram();
        sprintf(message, "node qty: %d\n", nodeCount);
        MESSAGE(message);
        return true;
    }
    else if (!strcmp(commandStr, "reboot"))
    {
        fortabIdx = 0;
        programIdx = 0;
        cmdLineIdx = 0;
        callSP = 0;
        numSP = 0;
        strSP = 0;
        FreeExprTrees();
        FreeSymtab();
        FreeProgram();
        InitDisplay();
        return true;
    }

    // init the lexer and parse the command line to create the IR
    if (GetNextToken(commandStr))
    {
        if (IsCommandLine(&commandLine, &isImmediate))
        {
            if (isImmediate)
            {
                // the command line has no line number so execute it and free any dynamic commands immediately
                cmdPtr = &commandLine.cmd;
                while (cmdPtr)
                {
                    if (!ExecCommand(cmdPtr, isImmediate))
                    {
                        return false;
                    }
                }
                FreeCommandLine(&commandLine);
                return true;
            }
            else
            {
                // check to see if this is a replacement of an existing line by number
                for (i = 0; i < programIdx; i++)
                {
                    if (commandLine.lineNum == Program[i].lineNum)
                    {
                        break;
                    }
                }
                Program[i] = commandLine;
                if (i == programIdx)
                {
                    //Program[i].lineNum = command.lineNum;
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

// execute commands in the program based on the command pointer, cmdPtr, which 
// will be iterated unless a command is executed that explicitly changes it
bool RunProgram(void)
{
    char tempStr[STRING_LEN];
    
    // default error string
    strcpy(errorStr, "execution error");
    
    // ready for more commands whether or not the program executes correctly
    ready = true;    
      
    // init the command pointer to the first command in the first command line
    if (programIdx != 0)
    {
        cmdPtr = &Program[0].cmd;
        cmdLineIdx = 0;
        while (cmdPtr != NULL)
        {
            // execute the command
            if (!ExecCommand(cmdPtr, ALL_COMMANDS))
            {
                sprintf(tempStr, " at line %d", cmdPtr->lineNum);
                strcat(errorStr, tempStr);
                return false;
            }
        }
    }
    else
    {
        strcpy(errorStr, "no program present");
        return false;
    }
          
    return true;
}

bool ListProgram(void)
{
    for (int i = 0; i < programIdx; i++)
    {
        if (Program[i].cmd.type != CT_NOP || strstr(Program[i].commandStr, "rem"))
        {
            strcpy(resultStr, Program[i].commandStr);
            PrintResult();
        }
    }
    ready = true;      
      
    return true;
}

// execute a possible list of commands
bool ExecCommand(Command *command, bool cmdLineOnly)
{
    Command *lastCmdPtr = cmdPtr;
    
    switch (command->type)
    {
        case CT_NOP: 
            break;
                
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
            if (!ExecPoke(&command->cmd.platformCmd))
                return false;
            break;                
                
        case CT_TONE: 
            if (!ExecTone(&command->cmd.platformCmd))
                return false;
            break;                
                
        case CT_BEEP: 
            if (!ExecBeep(&command->cmd.platformCmd))
                return false;
            break;                
                
        case CT_DISPLAY: 
            if (!ExecDisplay(&command->cmd.platformCmd))
                return false;
            break;                
                
        case CT_OUTCHAR: 
            if (!ExecOutchar(&command->cmd.platformCmd))
                return false;
            break;                
                
        case CT_RSEED: 
            if (!ExecRseed(&command->cmd.platformCmd))
                return false;
            break;                
                
        case CT_DELAY: 
            if (!ExecDelay(&command->cmd.platformCmd))
                return false;
            break;                
                
        case CT_DIM: 
            if (!ExecDim(&command->cmd.dimCmd))
                return false;
            break;                
    }
    PrintResult();
    if (cmdPtr == lastCmdPtr)
    {
        // if a command didn't change the command pointer, logically increment it
        cmdPtr = IterateCmdPtr(cmdLineOnly);                      
    }
    
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
                    switch (cmd->style)
                    {
                        case PS_DECIMAL:
                            sprintf(exprStr, "%f", numval);
                            sscanf(exprStr, "%d.%d", &intval, &decval);
                            if (decval == 0)
                            {
                                sprintf(exprStr, "%.f", numval);
                            }
                            break;
                        case PS_HEX:
                            sprintf(exprStr, "0x%x", (unsigned int)numval);
                            break;
                        case PS_ASCII:
                            sprintf(exprStr, "%c", (int)numval);
                            break;
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
    //ip++;
    
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

    return true;
}

// for : FOR Intvar '=' init TO to [STEP step]
bool ExecFor(ForCommand *cmd)
{
    float value;
    
    //Console("ExecFor()\n");
    
    // perform initial variable assignment then push for-command onto FOR stack
    if (EvaluateNumExpr(cmd->init, &value))
    {
        if (SymWriteNumvar(cmd->symbol, NULL, value))
        {
            fortab[fortabIdx++] = cmd;
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
        if (fortabIdx == 0)
        {
            strcpy(errorStr, "no matching for");
            return false;
        }
    }
    else
    {
        // use the for instruction on the top of the for stack for annonymous next
        if (fortabIdx == 0)
        {
            strcpy(errorStr, "no matching for");
            return false;
        }
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
                    // goto the first command in the command line following the for command
                    if (LineNum2CmdLineIdx(forInstr->lineNum))
                    {
                        cmdLineIdx++;
                        cmdPtr = &Program[cmdLineIdx].cmd;
                    }
                    else
                    {
                        return false;
                    }
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
            // goto the first command in the command line of the goto destination
            if (LineNum2CmdLineIdx((int)dest))
            {
                cmdPtr = &Program[cmdLineIdx].cmd;
                return true;
            }
            else
            {
                return false;
            }
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
            // push the pointer to the next command to be executed after the subroutine return onto the call stack
            callStack[callSP++] = IterateCmdPtr(false);
            
            // goto the first command in the command line of the gosub destination
            if (LineNum2CmdLineIdx((int)dest))
            {
                cmdPtr = &Program[cmdLineIdx].cmd;
                return true;
            }
        }
    }

    return false;
}

// return : RETURN
bool ExecReturn(void)
{
    
    //Console("ExecReturn()\n");
    
    // ensure the call stack isn't empty
    if (callSP == 0)
    {
        strcpy(errorStr, "return without gosub");
        return false;
    }
    // pop the next command to be executed from the call stack
    cmdPtr = callStack[--callSP];
    if (LineNum2CmdLineIdx(cmdPtr->lineNum))
    {
        return true;
    }
    
    return false;
}

// end : END
bool ExecEnd(void)
{
    // end the program by setting the command point to NULL
    cmdPtr = NULL;
    return true;
}

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

    return true;
}

bool ExecPoke(PlatformCommand *cmd)
{
    float numval;
    int addr, data;
    
    if (EvaluateNumExpr(cmd->arg1, &numval))
    {
        addr = (int)numval;
        if (EvaluateNumExpr(cmd->arg2, &numval))
        {
            data = (int)numval;
            MemWrite((uint16_t)addr, (uint8_t)data);
            return true;
        }
    }
    
    return false;
}

bool ExecTone(PlatformCommand *cmd)
{
    float numval;
    int freq, duration;
    
    if (EvaluateNumExpr(cmd->arg1, &numval))
    {
        freq = (int)numval;
        if (EvaluateNumExpr(cmd->arg2, &numval))
        {
            duration = (int)numval;
            Tone((uint16_t)freq, (uint16_t)duration);
            return true;
        }
    }
    
    return false;
}

bool ExecBeep(PlatformCommand *cmd)
{
    Tone((uint16_t)BEEP_TONE, (uint16_t)BEEP_DURATION);
    return true;
}

bool ExecDisplay(PlatformCommand *cmd)
{
    float numval;
    int value, displayQty;
    
    if (EvaluateNumExpr(cmd->arg1, &numval))
    {
        value = (int)numval;
        if (EvaluateNumExpr(cmd->arg2, &numval))
        {
            displayQty = (int)numval;
            Display((uint16_t)value, (uint8_t)displayQty);
            return true;
        }
    }
    
    return false;
}

bool ExecOutchar(PlatformCommand *cmd)
{
    float numval;
    
    if (EvaluateNumExpr(cmd->arg1, &numval))
    {
        sprintf(message, "%c", (int)numval);
        Console(message);
        return true;
    }
    
    return false;
}

bool ExecRseed(PlatformCommand *cmd)
{
    float numval;
    
    if (EvaluateNumExpr(cmd->arg1, &numval))
    {
        srand((unsigned int)numval);
        return true;
    }
    
    return false;
}

bool ExecDelay(PlatformCommand *cmd)
{
    float numval;
    int duration;
    
    if (EvaluateNumExpr(cmd->arg1, &numval))
    {
        duration = (int)numval;
        Delay((uint16_t)duration);
        return true;
    }
    
    return false;
}

// dim : DIM {Numvar | Strvar} '(' expr [',' expr]+ ')'
bool ExecDim(DimCommand *cmd)
{
    // evaluate the dim sizes
    for (int i = 0; i < SYM_DIM(cmd->varsym); i++)
    {
        if (!EvaluateNumExpr(cmd->dimSizeNodes[i], &SYM_DIMSIZES(cmd->varsym, i)))
        {
            strcpy(errorStr, "invalid dim expression");
            return false;
        }
    }

    return true;
}

bool ExecBuiltinFct(const char *name)
{
    if (!strcmp(name, "peek"))
    {
        return NumStackPush(MemRead((uint16_t)NumStackPop()));
    } 
    else if (!strcmp(name, "rnd"))
    {
        return NumStackPush(rand() % ((uint16_t)NumStackPop()));
    } 
    else if (!strcmp(name, "abs"))
    {
        return NumStackPush(fabsf(NumStackPop()));
    } 
    
    return false;       
}

// return the value of an expression based on its expr tree
bool EvaluateNumExpr(Node *exprTreeRoot, float *pValue)
{
    int oldStrSP = strSP;
    
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

// TODO: add call stack push/pop functions

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

