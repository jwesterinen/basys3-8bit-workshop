/*
 *  codegen.h   Machine code generator for asm16
 *
 *  These functions are called from the asm16 parser, asm16.y. There are 7 
 *  canonical instruction formats for all possible asm16 instructions,
 *  i.e. multiples of instructions can use the same basic format.
 *
 *  Canonical instruction formats:
 *
 *      Address format      bit structure                           instr type      semantics                       syntax
 *      ************************************************************************************************************************************************-
 
 *      1. register:        00000 aaa 0**** 000                     unary op        A <- <op> A                     <unop>      <reg>                       
 *                          00000 aaa 0++++ bbb                     direct op       A <- A <op> B                   <binop>     <dreg>,<sreg>               
 *                          00001 aaa 0++++ bbb                     indirect op     A <- A <op> [B]                 <binop>     <dreg>,[<sreg>]             
 *
 *      2. immed8:          11 +++ aaa ########                     op              A <- A <binop> ########         <binop>     <reg>,#<immed8>             
 *
 *      3. ZP:              00101 aaa ########                      load            A <- [00000000########]         mov         <reg>,[#<immed8>]           
 *                          00110 aaa ########                      store           [00000000########] <- A         mov         [#<immed8>],<reg>           
 *
 *      4. indexed5:        01001 aaa ##### bbb                     load            A <- [B+#####]                  mov         <dreg>,[<sreg>+<immed5>]    
 *                          01001 aaa 00001 110                     load            A <- [SP+0]                     pop         <reg>
 *                          01001 111 00001 110                     load            IP <- SP                        rts
 *                          01010 aaa ##### bbb                     store           [B+#####] <- A                  mov         [<dreg>+<immed5>],<sreg>    
 *                          01010 aaa ##### bbb                     store 0 index   [B+0] <- A                      mov         [<dreg>],<sreg>             
 *                          01010 aaa 00000 110                     store 0 index   [SP+0] <- A                     push        <reg>
 *
 *      5. call:            01110 111 00 ccc 110                    call            [SP] <- IP, IP <- C             jsr         <reg>                       
 *
 *      6. immed16:         00011 aaa 0++++ 000 ################    op              A <- A <op> <immed16>           <binop>     <reg>,@<immed16>
 *            
 *      6a. direct:         01101 aaa 0++++ 000 ################    op              A <- A <op> <addr16>            <binop>     <reg>,<addr16>              
 *                          01100 000 00000 bbb ################    store           <addr16> <- B                   mov         <addr16>,<reg>              
 *                          00011 111 00000 000 ################    jump            IP <- <addr16>                  jmp         <label>                     
 *                          01000 111 00000 110 ################    call            [SP] <- IP, IP <- <addr16>      jsr         <label>                     
 *
 *      7. IP relative:     1000 tttt ########                      branch          IP <- (IP+########)             <brcond>    <label>                     
 *                          1010 tttt ########	                    call            [B] <- A, IP <- (IP+########)   <bccond>    <label>
 *
 *      Possible new instructions:
 *
 *      6b. indirect        01111 aaa 0++++ 000 ################    indirect op     A <- A <op> [<addr16>]          <binop>     <reg>,[<addr16>]            
 *                          00010 aaa 00000 000 ################    indirect store  [<addr16>] <- A                 mov         [<addr16>],<reg>
 *
 *      8a. indexed8        01111 aaa ######## ################     indexed op      A <- A <op> [<addr16>+########] <binop>     <reg>,[<addr16>+<immed8>]              
 *                          00010 aaa ######## ################     indexed store   [<addr16>+########] <- A        mov         [<addr16+immed8>],<reg>              
 *       - 
 *
 *      Legend:
 *          aaa: destination register
 *          bbb: source register
 *          ccc: address register
 *          ++++: ALU operators
 *          #####: 5-bit immediate
 *          ########: 8-bit immediate
 *          ################: 16-bit immediate
 *          tttt: branch condition
 *
 *      Note: Immediate values can also be defined symbols using the .define directive, e.g. "mov ax,0x1234" is the same instruction as "mov ax,foo" with ".define foo 0x1234"
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "symtab.h"
#include "asm16.h"

extern FILE* yyout;
extern FILE *yyerfp;

void GenRegCode(unsigned char opCode, unsigned char destReg, unsigned char aluOp, unsigned char srcReg, int isUnary)
{
    unsigned short instr = 0x0000;
    
    instr |= (opCode    << OPCODE_SHIFT)    & OPCODE_MASK;
    instr |= (destReg   << DREG_SHIFT)      & DREG_MASK;
    instr |= (aluOp     << ALU_OP_SHIFT)    & ALU_OP_MASK;
    instr |= (srcReg    << SREG_SHIFT)      & SREG_MASK;
    fprintf(yyout, "%04X\n", instr);
}

void GenImmed8Code(unsigned char opCode, unsigned char aluOp, unsigned char destReg, int immed8)
{
    unsigned short instr = 0x0000;
    
    // immediate value must fit in 8 bits
    if (immed8 > 0xff)
    {
        yyerror("Immediate number is too large to fit in 8 bits.");
        fprintf(yyout, "****\n");
        return;
    }                
    instr |= (opCode    << OPCODE_SHIFT)        & OPCODE_MASK;
    instr |= (aluOp     << IMM8_ALU_OP_SHIFT)   & IMM8_ALU_OP_MASK;
    instr |= (destReg   << DREG_SHIFT)          & DREG_MASK;
    instr |= (immed8    << IMMED8_SHIFT)        & IMMED8_MASK;
    fprintf(yyout, "%04X\n", instr);
}

void GenZPCode(unsigned char opCode, unsigned char destReg, int zpAddr)
{
    unsigned short instr = 0x0000;
    
    // the ZP address must fit in 8 bits
    if (zpAddr > 0xff)
    {
        yyerror("Immediate number is too large to fit in 8 bits.");
        fprintf(yyout, "****\n");
        return;
    }
    instr |= (opCode    << OPCODE_SHIFT)    & OPCODE_MASK;
    instr |= (destReg   << DREG_SHIFT)      & DREG_MASK;
    instr |= (zpAddr    << IMMED8_SHIFT)    & IMMED8_MASK;
    fprintf(yyout, "%04X\n", instr);
}

void GenIndexedCode(unsigned char opCode, unsigned char destReg, int index5, unsigned char srcReg)
{
    unsigned short instr = 0x0000;
    
    // the index must fit in 5 bits
    if (index5 > 0x1f)
    {
        yyerror("The index is too large to fit in 5 bits.");
        fprintf(yyout, "****\n");
        return;
    }
    instr |= (opCode    << OPCODE_SHIFT)    & OPCODE_MASK;
    instr |= (destReg   << DREG_SHIFT)      & DREG_MASK;
    instr |= (index5    << INDEX5_SHIFT)    & INDEX5_MASK;
    instr |= (srcReg    << SREG_SHIFT)      & SREG_MASK;
    fprintf(yyout, "%04X\n", instr);
}

void GenCallCode(unsigned char opCode, unsigned char destReg, unsigned char addrReg, unsigned char srcReg)
{
    unsigned short instr = 0x0000;
    
    instr |= (opCode    << OPCODE_SHIFT)    & OPCODE_MASK;
    instr |= (destReg   << DREG_SHIFT)      & DREG_MASK;
    instr |= (addrReg   << AREG_SHIFT)      & AREG_MASK;
    instr |= (srcReg    << SREG_SHIFT)      & SREG_MASK;
    fprintf(yyout, "%04X\n", instr);
}

void GenDirectCode(unsigned char opCode, unsigned char destReg, unsigned char aluOp, unsigned char srcReg, int value16)
{
    unsigned short instr = 0x0000;
    
    // immediate value must fit in 16 bits
    if (value16 > 0xffff)
    {
        yyerror("The immediate value is too large to fit in 16 bits.");
        fprintf(yyout, "****\n");
        return;
    }                
    instr |= (opCode    << OPCODE_SHIFT)    & OPCODE_MASK;
    instr |= (destReg   << DREG_SHIFT)      & DREG_MASK;
    instr |= (aluOp     << ALU_OP_SHIFT)    & ALU_OP_MASK;
    instr |= (srcReg    << SREG_SHIFT)      & SREG_MASK;
    fprintf(yyout, "%04X ", instr);
    fprintf(yyout, "%04X\n", value16);
}

void GenIPRelativeCode(unsigned char opCode, unsigned char condition, int offset)
{
    unsigned short instr = 0x0000;
        
    if (offset > 0xff)
    {
        yyerror("The offset is too large to fit in 8 bits.");
        fprintf(yyout, "****\n");
        return;
    }
    instr |= (opCode    << OPCODE_SHIFT)    & OPCODE_MASK;
    instr |= (condition << BRCOND_SHIFT)    & BRCOND_MASK;
    instr |= (offset    << IMMED8_SHIFT)    & IMMED8_MASK;
    fprintf(yyout, "%04X\n", instr);
}

void GenErrorCode()
{
    fprintf(yyout, "****\n");
}

// end of codegen.c

