/*
 *	asm16 _ cpu16 assembly language grammar
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "symtab.h"
#include "asm16.h"
#include "codegen.h"

int yylex(void);
extern FILE* yyout;

#define NAME(x)     (((struct Symbol *)x)->s_name)
#define VALUE(x)     (((struct Symbol *)x)->s_value)

extern int asm_pass;
extern unsigned short cur_addr;
extern int verbose;

unsigned char regId, srcReg, destReg, aluop, immed8, brcond;
unsigned short instr;

%}

// parser stack type union
%union  {
            struct Symbol *y_sym;   // Identifier
            int y_num;              // Immediate
        }

%token <y_sym> Identifier
%token <y_sym> Label
%token <y_num> Immediate
%token INT
%token AX
%token BX
%token CX
%token DX
%token EX
%token BP
%token SP
%token IP
%token ZERO
%token LOADA
%token INC
%token DEC
%token ASL
%token LSR
%token ROL
%token ROR
%token OR
%token AND
%token XOR
%token MOV
%token ADD
%token SUB
%token ADC
%token SBB
%token PUSH
%token POP
%token RTS
%token JSR
%token JMP
%token BRA
%token BCC
%token BCS
%token BNZ
%token BZ
%token BPL
%token BMI
%token BRAS
%token BCCS
%token BCSS
%token BNZS
%token BZS
%token BPLS
%token BMIS
%token RESET
%token ORIGIN
%token DEFINITION
%token DEFINE_WORD;
%token DEFINE_ZPWORD;

%%

program
    :   commands
        {
            if (asm_pass == 1)
            {
                if (verbose)
                {
                    fprintf(stdout, "last instruction address = 0x%04X\n", cur_addr);
                    fprintf(stdout, "...pass 1 complete\n");
                }
            }
            else
                if (verbose)
                {
                    fprintf(stdout, "...pass 2 complete\n");
                }
        }
commands
	: command
	| commands command
	
command
    : origin
    | definition
    | label
    | zp_word_alloc
    | word_alloc
    | one_word_instr
        {
            cur_addr++;
        }
    | two_word_instr
        {
            cur_addr++;
            cur_addr++;
        }

origin
    : ORIGIN Immediate
        {
            cur_addr = $2;
        }

definition
    : DEFINITION Identifier Immediate
        {
            if (asm_pass == 1)
            {
                if (s_define($2, ST_ID, $3))
                {
                    if (verbose)
                    {
                        fprintf(stdout, "%s is defined as 0x%04X\n", NAME($2), VALUE($2));
                    }
                }
            }
        }
            
label
    : Identifier':'
        {
            if (asm_pass == 1)
            {
                if (s_define($1, ST_LABEL, cur_addr))
                {
                    if (verbose)
                    {
                        fprintf(stdout, "Label %s is at address 0x%04X\n", NAME($1), VALUE($1));
                    }
                }
            }
        }
        
zp_word_alloc
    : DEFINE_ZPWORD Identifier
        {
            if (asm_pass == 1)
            {
                if (s_alloc_ram($2, ST_ZPRAM_ADDR))
                {
                    if (verbose)
                    {
                        fprintf(stdout, "%s was allocated ZP offset 0x%02X\n", NAME($2), VALUE($2));
                    }
                }
            }
        }

word_alloc
    : DEFINE_WORD Identifier
        {
            if (asm_pass == 1)
            {
                if (s_alloc_ram($2, ST_RAM_ADDR))
                {
                    if (verbose)
                    {
                        fprintf(stdout, "%s was allocated address 0x%04X\n", NAME($2), VALUE($2));
                    }
                }
            }
        }

one_word_instr
    : unary_instr 
    | reg_instr
    | reg_indir_instr 
    | immediate8_instr
    | load_zp_instr
    | store_zp_instr
    | load_index5_instr
    | store_index5_instr
    | store_reg_instr   
    | ip_rel_branch_instr
    | ip_rel_call_instr    	
    | push_instr
    | pop_instr
    | rts_instr
    | reg_call_instr


two_word_instr
    : immediate16_instr
    | direct_instr 
    | store_direct_instr 
    | direct_call_instr
    | direct_jump_instr


unary_instr           
    : unop dreg
        {
            // unary
            if (asm_pass == 2)
            {
                // use the register instruction format with 0 as source reg and specify unary
                GenRegCode(REG_OPCODE, destReg, aluop, 0, 1);
            }
        }
    | unop error

reg_instr             
    : binop dreg','sreg
        {
            if (asm_pass == 2)
            {
                // use the register instruction format and specify not unary
                GenRegCode(REG_OPCODE, destReg, aluop, srcReg, 0);
            }
        }
    | MOV dreg','sreg
        {
            if (asm_pass == 2)
            {
                // use the register instruction format and specify not unary
                GenRegCode(REG_OPCODE, destReg, MOV_ALU_OP, srcReg, 0);
            }
        }
    | error

reg_indir_instr       
    : binop dreg',''['sreg']'
        {
            if (asm_pass == 2)
            {
                // use the register instruction format and specify not unary
                GenRegCode(REG_INDIR_OPCODE, destReg, aluop, srcReg, 0);
            }
        }
    | MOV dreg',''['sreg']'
        {
            if (asm_pass == 2)
            {
                // use the register instruction format and specify not unary
                GenRegCode(REG_INDIR_OPCODE, destReg, MOV_ALU_OP, srcReg, 0);
            }
        }

immediate8_instr          
    : binop dreg',''#'Immediate
        {
            if (asm_pass == 2)
            {
                // use immed8 instruction formaat
                GenImmed8Code(IMMEDIATE8_OPCODE, aluop, destReg, $5);
            }
        }
    | binop dreg',''#'Identifier
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($5, ST_ID))
                {
                    // use immed8 instruction format
                    GenImmed8Code(IMMEDIATE8_OPCODE, aluop, destReg, VALUE($5));
                }
            }
        }
    | MOV dreg',''#'Immediate
        {
            if (asm_pass == 2)
            {
                // use immed8 instruction format
                GenImmed8Code(IMMEDIATE8_OPCODE, MOV_ALU_OP, destReg, $5);
            }
        }
    | MOV dreg',''#'Identifier
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($5, ST_ID))
                {
                    // use immed8 instruction format
                    GenImmed8Code(IMMEDIATE8_OPCODE, MOV_ALU_OP, destReg, VALUE($5));
                }
            }
        }
    
load_zp_instr         
    : MOV dreg',''[''#'Immediate']'
        {
            if (asm_pass == 2)
            {
                // use ZP instruction format
                GenZPCode(ZP_LOAD_OPCODE, destReg, $6);
            }
        }
    | MOV dreg',''[''#'Identifier']'
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($6, ST_ZPRAM_ADDR))
                {
                    // use ZP instruction format
                    GenZPCode(ZP_LOAD_OPCODE, destReg, VALUE($6));
                }
            }
        }

store_zp_instr        
    : MOV '[''#'Immediate']'','sreg
        {
            if (asm_pass == 2)
            {
                // use ZP instruction format
                GenZPCode(ZP_STORE_OPCODE, srcReg, $4);
            }
        }
    | MOV '[''#'Identifier']'','sreg
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($4, ST_ZPRAM_ADDR))
                {
                    // use ZP instruction format
                    GenZPCode(ZP_STORE_OPCODE, srcReg, VALUE($4));
                }
            }
        }

load_index5_instr    
    : MOV dreg',''['sreg'+'Immediate']'
        {
            if (asm_pass == 2)
            {
                // use Indexed instruction format
                GenIndexedCode(INDEX5_LOAD_OPCODE, destReg, $7, srcReg);
            }
        }
    | MOV dreg',''['sreg'+'Identifier']'
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($7, ST_ID))
                {
                    // use Indexed instruction format
                    GenIndexedCode(INDEX5_LOAD_OPCODE, destReg, VALUE($7), srcReg);
                }
            }
        }

store_index5_instr   
    : MOV '['dreg'+'Immediate']'','sreg
        {
            if (asm_pass == 2)
            {
                // use Indexed instruction format
                GenIndexedCode(INDEX5_STORE_OPCODE, srcReg, $5, destReg);
            }
        }
    | MOV '['dreg'+'Identifier']'','sreg
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($5, ST_ID))
                {
                    // use Indexed instruction format
                    GenIndexedCode(INDEX5_STORE_OPCODE, srcReg, VALUE($5), destReg);
                }
            }
        }

store_reg_instr   
    : MOV '['dreg']'','sreg
        {
            if (asm_pass == 2)
            {
                // use Indexed instruction format with 0 index
                GenIndexedCode(INDEX5_STORE_OPCODE, srcReg, 0, destReg);
            }
        }

ip_rel_branch_instr     
    : bcond Identifier
        {
            if (asm_pass == 2)
            {
                // check for defined Identifier
                if (chk_identifier($2, ST_LABEL))
                {
                    // use IP-relative instruction format
                    int offset = VALUE($2) - (cur_addr + 1);
                    GenIPRelativeCode(IP_REL_BRANCH_OPCODE, brcond, offset);
                }
            }
        }

ip_rel_call_instr      
    : bscond Identifier
        {
            if (asm_pass == 2)
            {
                // check for defined Identifier
                if (chk_identifier($2, ST_LABEL))
                {
                    // use IP-relative instruction format
                    int offset = VALUE($2) - (cur_addr + 1);
                    GenIPRelativeCode(IP_REL_CALL_OPCODE, brcond, offset);
                }
            }
        }

push_instr
    : PUSH dreg
        {
            if (asm_pass == 2)
            {
                // use Indexed instruction format with SP as source and no index
                GenIndexedCode(INDEX5_STORE_OPCODE, destReg, 0, REG_SP);
            }
        }
    
pop_instr 
    : POP dreg
        {
            if (asm_pass == 2)
            {
                // use Indexed instruction format with SP as source 
                // and 1 as index to compensate for the way the stack works
                GenIndexedCode(INDEX5_LOAD_OPCODE, destReg, 1, REG_SP);
            }
        }
    
rts_instr
    : RTS
        {
            if (asm_pass == 2)
            {
                // use Indexed instruction format with IP as dest, SP as source, 
                // and 1 as index to compensate for the implicit inc of the IP before the call
                GenIndexedCode(INDEX5_LOAD_OPCODE, REG_IP, 1, REG_SP);
            }
        }
    
reg_call_instr
    : JSR reg
        {
            if (asm_pass == 2)
            {
                // use Call instruction format
                GenCallCode(REG_CALL_OPCODE, REG_IP, regId, REG_SP);
            }
        }
    
immediate16_instr         
    :  binop dreg',''@'Immediate
        {
            if (asm_pass == 2)
            {
                // use the Direct instruction format with no source reg
                GenDirectCode(IMMEDIATE16_OPCODE, destReg, aluop, 0, $5);
            }
        }
    |  binop dreg',''@'Identifier
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($5, ST_ID))
                {
                    // use the Direct instruction format with no source reg
                    GenDirectCode(IMMEDIATE16_OPCODE, destReg, aluop, 0, VALUE($5));
                }
            }
        }
    |  MOV dreg',''@'Immediate
        {
            if (asm_pass == 2)
            {
                // use the Direct instruction format with no source reg
                GenDirectCode(IMMEDIATE16_OPCODE, destReg, MOV_ALU_OP, 0, $5);
            }
        }
    |  MOV dreg',''@'Identifier
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($5, ST_ID|ST_LABEL))
                {
                    // use the Direct instruction format with no source reg
                    GenDirectCode(IMMEDIATE16_OPCODE, destReg, MOV_ALU_OP, 0, VALUE($5));
                }
            }
        }

direct_instr          
    :  binop dreg','Immediate
        {
            if (asm_pass == 2)
            {
                // use the Direct instruction format with no source reg
                GenDirectCode(DIRECT_OPCODE, destReg, aluop, 0, $4);
            }
        }
    |  binop dreg','Identifier
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($4, ST_ID|ST_RAM_ADDR))
                {
                    // use the Direct instruction format with no source reg
                    GenDirectCode(DIRECT_OPCODE, destReg, aluop, 0, VALUE($4));
                }
            }
        }
    |  MOV dreg','Immediate
        {
            if (asm_pass == 2)
            {
                // use the Direct instruction format with no source reg
                GenDirectCode(DIRECT_OPCODE, destReg, MOV_ALU_OP, 0, $4);
            }
        }
    |  MOV dreg','Identifier
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($4, ST_ID|ST_RAM_ADDR))
                {
                    // use the Direct instruction format with no source reg
                    GenDirectCode(DIRECT_OPCODE, destReg, MOV_ALU_OP, 0, VALUE($4));
                }
            }
        }

store_direct_instr    
    :  MOV Immediate','sreg
        {
            if (asm_pass == 2)
            {
                // use the Direct instruction format with no destination reg nor ALU opcode
                GenDirectCode(DIRECT_STORE_OPCODE, 0, 0, srcReg, $2);
            }
        }
    |  MOV Identifier','sreg
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($2, ST_ID|ST_RAM_ADDR))
                {
                    // use the Direct instruction format with no destination reg nor ALU opcode
                    GenDirectCode(DIRECT_STORE_OPCODE, 0, 0, srcReg, VALUE($2));
                }
            }
        }

direct_call_instr
    : JSR Identifier
        {
            if (asm_pass == 2)
            {
                // check for defined Identifier
                if (chk_identifier($2, ST_LABEL))
                {
                    // use the Direct instruction format with the IP as the destinition, no ALU opcode, the SP as the source reg, and the address of the Identifier
                    GenDirectCode(DIRECT_CALL_OPCODE, REG_IP, 0, REG_SP, VALUE($2));
                }
            }
        }

direct_jump_instr
    : JMP Identifier
        {
            if (asm_pass == 2)
            {
                if (chk_identifier($2, ST_LABEL))
                {
                    // use the Direct instruction format with the IP as the destinition, the MOV ALU opcode, no source reg, and the address of the Identifier
                    GenDirectCode(DIRECT_JUMP_OPCODE, REG_IP, MOV_ALU_OP, 0, VALUE($2));
                }
            }
        }
    
sreg                  
    : reg   {srcReg = regId;}

dreg                  
    : reg   {destReg = regId;}

unop                  
    : ZERO  {aluop = OP_ZERO;}
    | LOADA {aluop = OP_LOAD_A;}
    | INC   {aluop = OP_INC;}
    | DEC   {aluop = OP_DEC;} 
    | ASL   {aluop = OP_ASL;}
    | LSR   {aluop = OP_LSR;}
    | ROL   {aluop = OP_ROL;} 
    | ROR   {aluop = OP_ROR;}
    
binop    
    : OR    {aluop = OP_OR;}
    | AND   {aluop = OP_AND;}
    | XOR   {aluop = OP_XOR;}
    | ADD   {aluop = OP_ADD;}
    | SUB   {aluop = OP_SUB;}
    | ADC   {aluop = OP_ADC;}
    | SBB   {aluop = OP_SBB;}

bcond                 
    : BRA   {brcond = BRCOND_ALLWAYS;} 
    | BCC   {brcond = BRCOND_CARRY_CLEAR;} 
    | BCS   {brcond = BRCOND_CARRY_SET;} 
    | BNZ   {brcond = BRCOND_NOT_ZERO;} 
    | BZ    {brcond = BRCOND_ZERO;} 
    | BPL   {brcond = BRCOND_PLUS;} 
    | BMI   {brcond = BRCOND_MINUS;} 


bscond                
    : BRAS  {brcond = BRCOND_ALLWAYS;} 
    | BCCS  {brcond = BRCOND_CARRY_CLEAR;} 
    | BCSS  {brcond = BRCOND_CARRY_SET;} 
    | BNZS  {brcond = BRCOND_NOT_ZERO;} 
    | BZS   {brcond = BRCOND_ZERO;} 
    | BPLS  {brcond = BRCOND_PLUS;} 
    | BMIS  {brcond = BRCOND_MINUS;} 

reg                   
    : AX    {regId = REG_AX;}
    | BX    {regId = REG_BX;}
    | CX    {regId = REG_CX;}
    | DX    {regId = REG_DX;}
    | EX    {regId = REG_EX;}
    | BP    {regId = REG_BP;}
    | SP    {regId = REG_SP;}
    | IP    {regId = REG_IP;}
    
// end of asm16.y

