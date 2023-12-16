%{
/*
 *	hcc -- hack C compiler lexical analysis
 */

#include "y.tab.h"
#include "error.h"
#include "symtab.h"
%}

letter          [a-zA-Z_]
digit           [0-9]
letter_or_digit [a-zA-Z_0-9]
hextet          [a-fA-F0-9]
hex_prefix      0[xX]
newline         [\n]
blank           [ \t]
other           .

%%

^"#"{blank}*{digit}+({blank}+.*)?\n	    yymark();

"break"                     return BREAK;
"continue"                  return CONTINUE;
"else"                      return ELSE;
"if"                        return IF;
"int"                       return INT;
"void"                      return VOID;
"return"                    return RETURN;
"while"                     return WHILE;

">="                        return GE;
"<="                        return LE;
"=="                        return EQ;
"!="                        return NE;
"+="                        return PE;
"-="                        return ME;
"*="                        return TE;
"/="                        return DE;
"%="                        return RE;
"&="                        return AE;
"|="                        return OE;
"++"                        return PP;
"--"                        return MM;
"||"                        return OR;
"&&"                        return AND;

{letter}{letter_or_digit}*  {
                                s_lookup(Identifier);
                                return Identifier;
                            }

{digit}+                    {
                                s_lookup(Constant);
                                return Constant;
                            }

{hex_prefix}{hextet}+       {
                                s_lookup(Constant);
                                return Constant;
                            }
                            
{newline}                   yylineno++;
{blank}+                    ;

{other}                     return yytext[0];

