/*  asm16.h

    This file contains the definitions used in code generation
    
    {"fmt":"~binop ~reg,~reg",   "bits":["00000",1,"01",0,2]},
    {"fmt":"~unop ~reg",         "bits":["00000",1,"00",0,"000"]},
    {"fmt":"~binop ~reg,[~reg]", "bits":["00001",1,"01",0,2]},
    {"fmt":"mov [~reg],~reg",    "bits":["01010",0,"00000",1]},
    {"fmt":"mov [~reg+~imm5],~reg", "bits":["01010",0,1,2]},
    {"fmt":"mov ~reg,[~reg+~imm5]", "bits":["01001",0,2,1]},

    {"fmt":"~binop ~reg,[~imm16]",  "bits":["01101",1,"01",0,"000",2]},
    {"fmt":"mov [~imm16],~reg",  "bits":["0110000000000",1,0]},
    {"fmt":"mov ~reg,[#~imm8]",   "bits":["00101",0,1]},
    {"fmt":"mov [#~imm8],~reg",   "bits":["00110",1,0]},
    {"fmt":"~binop ~reg,#~imm8", "bits":["11",0,1,2]},
    {"fmt":"~binop ~reg,@~imm16","bits":["00011",1,"01",0,"000",2]},

    {"fmt":"push ~reg",          "bits":["01010",0,"00000","110"]},
    {"fmt":"pop ~reg",           "bits":["01001",0,"00001","110"]},
    {"fmt":"rts",                "bits":["01001","111","00001","110"]},
    {"fmt":"jsr ~reg",           "bits":["01110","111","00",0,"110"]},
    {"fmt":"jsr ~imm16",         "bits":["01000","111","00000","110",0]},
    {"fmt":"jmp ~imm16",         "bits":["0001111101011000",0]},

    {"fmt":"bra ~rel8",          "bits":["10000000",0]},
    {"fmt":"bcc ~rel8",          "bits":["10000001",0]},
    {"fmt":"bcs ~rel8",          "bits":["10001001",0]},
    {"fmt":"bnz ~rel8",          "bits":["10000010",0]},
    {"fmt":"bz ~rel8",           "bits":["10001010",0]},
    {"fmt":"bpl ~rel8",          "bits":["10000100",0]},
    {"fmt":"bmi ~rel8",          "bits":["10001100",0]},

    {"fmt":"bsr ~rel8",          "bits":["10100000",0]},
    {"fmt":"bscc ~rel8",         "bits":["10100001",0]},
    {"fmt":"bscs ~rel8",         "bits":["10101001",0]},
    {"fmt":"bsnz ~rel8",         "bits":["10100010",0]},
    {"fmt":"bsz ~rel8",          "bits":["10101010",0]},
    {"fmt":"bspl ~rel8",         "bits":["10100100",0]},
    {"fmt":"bsmi ~rel8",         "bits":["10101100",0]},

    {"fmt":"reset",              "bits":["1011100011111111"]}

*/    

// register IDs are 3 bits
enum regIds
{
    REG_AX,
    REG_BX,
    REG_CX,
    REG_DX,
    REG_EP,
    REG_BP,
    REG_SP,
    REG_IP
};
    
// ALU operator IDs are 4 bits    
enum AluOpIds
{
    OP_ZERO,
    OP_LOAD_A,
    OP_INC,
    OP_DEC,
    OP_ASL,
    OP_LSR,
    OP_ROL,
    OP_ROR,
    OP_OR,
    OP_AND,
    OP_XOR,
    OP_LOAD_B,
    OP_ADD,
    OP_SUB,
    OP_ADC,
    OP_SBB
};

// one word instruction opcode definitions
#define REG_OPCODE              0b00000
#define REG_INDIR_OPCODE        0b00001
#define IMMEDIATE8_OPCODE       0b11000
#define ZP_LOAD_OPCODE          0b00101
#define ZP_STORE_OPCODE         0b00110
#define INDEX5_LOAD_OPCODE      0b01001
#define INDEX5_STORE_OPCODE     0b01010
#define IP_REL_BRANCH_OPCODE    0b10000
#define IP_REL_CALL_OPCODE      0b10100
#define REG_CALL_OPCODE         0b01110

// two word instruction opcode definitions
#define IMMEDIATE16_OPCODE      0b00011
#define DIRECT_OPCODE           0b01101
#define DIRECT_STORE_OPCODE     0b01100
#define DIRECT_JUMP_OPCODE      0b00011
#define DIRECT_CALL_OPCODE      0b01000

// instruction field masks and shifts
#define MOV_ALU_OP              OP_LOAD_B    
#define ALU_OP_MASK             0b0000000001111000
#define ALU_OP_SHIFT            3
#define IMM8_ALU_OP_MASK        0b0011110000000000
#define IMM8_ALU_OP_SHIFT       11 
#define ZP_ALU_OP_MASK          0b0000011110000000
#define ZP_ALU_OP_SHIFT         7 
#define SREG_MASK               0b0000000000000111
#define SREG_SHIFT              0
#define DREG_MASK               0b0000011100000000
#define DREG_SHIFT              8
#define AREG_MASK               0b0000000000111000
#define AREG_SHIFT              3
#define OPCODE_MASK             0b1111100000000000
#define OPCODE_SHIFT            11
#define IMMED8_MASK             0b0000000011111111
#define IMMED8_SHIFT            0 
#define INDEX5_MASK             0b0000000011111000
#define INDEX5_SHIFT            3 
#define BRCOND_MASK             0b0000111100000000
#define BRCOND_SHIFT            8

// branch condition definitions
#define BRCOND_ALLWAYS          0b0000
#define BRCOND_CARRY_CLEAR      0b0001
#define BRCOND_CARRY_SET        0b1001
#define BRCOND_NOT_ZERO         0b0010
#define BRCOND_ZERO             0b1010
#define BRCOND_PLUS             0b0100
#define BRCOND_MINUS            0b1100

