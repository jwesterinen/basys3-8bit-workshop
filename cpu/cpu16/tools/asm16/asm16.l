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
"ex"                        return EX;
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
"pop"                       return POP;
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
"bras"                      return BRAS;
"bccs"                      return BCCS;
"bcss"                      return BCSS;
"bnzs"                      return BNZS;
"bzs"                       return BZS;
"bpls"                      return BPLS;
"bmis"                      return BMIS;
"reset"                     return RESET;
".org"                      return ORIGIN;

{letter}{letter_or_digit}*: {
                                if (asm_pass == 1)
                                {
                                    yylval.y_sym = s_create(yytext);
                                }
                                return Label;
                            }

{letter}{letter_or_digit}*  {
                                if (asm_pass == 2)
                                {
                                    yylval.y_sym = s_find(yytext);
                                }
                                return Identifier;
                            }

{digit}+                    {
                                yylval.y_num = atoi(yytext);
                                return Immediate;
                            }

{hex_prefix}{hextet}+       {
                                yylval.y_num = strtol(yytext, NULL, 16);
                                return Immediate;
                            }
                            
{comment_header}.*$         ;

{newline}                   yylineno++;
{blank}+                    ;

{other}                     return yytext[0];

