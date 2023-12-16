%{
/*
 *	asm16 -- cpu16 assembler lexical analysis
 */

#include "y.tab.h"
#include "error.h"
#include "symtab.h"
extern int asm_pass;
%}

letter          [a-zA-Z_]
digit           [0-9]
letter_or_digit [a-zA-Z_0-9]
hextet          [a-fA-F0-9]
hex_prefix      0[xX]
neg             -
comment_header  ;
newline         [\n]
blank           [ \t]
other           .

%%

^"#"{blank}*{digit}+({blank}+.*)?\n	    yymark();

"ax"                        return AX;
"bx"                        return BX;
"cx"                        return CX;
"dx"                        return DX;
"ep"                        return EP;
"bp"                        return BP;
"sp"                        return SP;
"ip"                        return IP;
"zero"                      return ZERO;
"loada"                     return LOADA;
"inc"                       return INC;
"dec"                       return DEC;
"asl"                       return ASL;
"lsr"                       return LSR;
"rol"                       return ROL;
"ror"                       return ROR;
"or"                        return OR;
"and"                       return AND;
"xor"                       return XOR;
"mov"                       return MOV;
"add"                       return ADD;
"sub"                       return SUB;
"adc"                       return ADC;
"sbb"                       return SBB;
"push"                      return PUSH;
"pushe"                     return PUSHE;
"pop"                       return POP;
"pope"                      return POPE;
"rts"                       return RTS;
"jsr"                       return JSR;
"jmp"                       return JMP;
"bra"                       return BRA;
"bcc"                       return BCC;
"bcs"                       return BCS;
"bnz"                       return BNZ;
"bz"                        return BZ;
"bpl"                       return BPL;
"bmi"                       return BMI;
"bsr"                       return BSR;
"bscc"                      return BSCC;
"bscs"                      return BSCS;
"bsnz"                      return BSNZ;
"bsz"                       return BSZ;
"bspl"                      return BSPL;
"bsmi"                      return BSMI;
"reset"                     return RESET;
".org"                      return ORIGIN;
".define"                   return DEFINITION;
".dw"                       return DEFINE_WORD;
".dz"                       return DEFINE_ZPWORD;
".ds"                       return DEFINE_STORAGE;

{letter}{letter_or_digit}*  {
                                s_lookup(Identifier);
                                return Identifier;
                            }

{neg}?{digit}+              {
                                yylval.y_num = atoi(yytext) & 0xffff;
                                return Immediate;
                            }

{neg}?{hex_prefix}{hextet}+ {
                                yylval.y_num = strtol(yytext, NULL, 16) & 0xffff;
                                return Immediate;
                            }
                            
{comment_header}.*$         ;

{newline}                   yylineno++;
{blank}+                    ;

{other}                     return yytext[0];

