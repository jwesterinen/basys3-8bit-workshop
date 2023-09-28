/*
 *  codegen.h   Machine code generator functions for asm16
 *
 */

void GenRegCode(unsigned char opCode, unsigned char destReg, unsigned char aluOp, unsigned char srcReg, unsigned isUnary);
void GenImmed8Code(unsigned char opCode, unsigned char aluOp, unsigned char destReg, int immed8);
void GenZPCode(unsigned char opCode, unsigned char destReg, int immed8);
void GenIndexedCode(unsigned char opCode, unsigned char destReg, int immed5, unsigned char srcReg);
void GenCallCode(unsigned char opCode, unsigned char destReg, unsigned char addrReg, unsigned char srcReg);
void GenDirectCode(unsigned char opCode, unsigned char destReg, unsigned char aluOp, unsigned char srcReg, int immed16);
void GenIPRelativeCode(unsigned char opCode, unsigned char condition, int offset);
void GenErrorCode();

