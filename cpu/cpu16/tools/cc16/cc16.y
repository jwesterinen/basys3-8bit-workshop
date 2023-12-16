/*
 *	hcc - a hack C compiler grammar
 *	syntax analysis with error recovery
 * 	(s/r conflict: one on ELSE, one on error)
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "symtab.h"
#include "gen.h"
int yylex(void);

#define OFFSET(x)   (((struct Symtab *)x)->s_offset)
#define NAME(x)     (((struct Symtab *)x)->s_name)
#define TYPE(x)     (((struct Symtab *)x)->s_type)

int is_void_expr = 0;
%}

/*
 * parser stack type union
 */
 
%union  {
            struct Symtab *y_sym;   // Identifier
            char *y_str;            // Constant, type  
            int y_num;              // count
            int y_lab;              // label
        }

/*
 *	terminal symbols
 */

%token  <y_sym> Identifier
%token  <y_str> Constant
%token  INT
%token  VOID
%token  IF
%token  ELSE
%token  WHILE
%token  BREAK
%token  CONTINUE
%token  RETURN
%token  ';'
%token  '('
%token  ')'
%token  '{'
%token  '}'
%token  '+'
%token  '-'
%token  '*'
%token  '/'
%token  '%'
%token  '~'
%token  '>'
%token  '<'
%token  GE
%token  LE
%token  EQ
%token  NE
%token  '&'
%token  '^'
%token  '|'
%token  '='
%token  PE
%token  ME
%token  TE
%token  DE
%token  RE
%token  AE
%token  OE
%token  PP
%token  MM
%token  ','

// TODO: add <<, >>, and %

/*
 *	typed non-terminal symbols
 */

%type   <y_sym> function_definition function_declaration optional_parameter_list parameter_declaration_list parameter_declaration parameter_declarator array_lhs_spec pointer_lhs_spec binary
%type   <y_num> argument_list pointer
%type   <y_lab> if_prefix loop_prefix

/*
 *	precedence table
 */

%right	'=' PE ME TE DE RE AE OE
%left   LOR
%left   LAND
%left   OR
%left   AND
%left	'|'
%left	'^'
%left	'&'
%left	EQ NE
%left	'<' '>' GE LE
%left	'+' '-'
%left	'*' '/' '%'
%right  '!' '~'
%right	PP MM

%%

program
	:   
	    {
	        init();
	        gen_begin_prog();
	    }
	  definitions
	    {
	        end_program();
	    }

definitions
	: definition
	| definitions definition
	    {
	        yyerrok;
	    }
	| error
	| definitions error

definition
	: function_definition
	| declaration

function_definition
	: function_declaration
	    {
	        l_max = 0;
	        $<y_lab>$ = gen_entry($1);
	    }
	  compound_statement
	    {
	        all_func($1);
	        gen_return(OP_RETURN, "end of function");
	        fix_entry($1, $<y_lab>2);
	    }

function_declaration
	: INT Identifier '(' 
	    {
	        make_func($2, 1);
	        blk_push();
	    }
	  optional_parameter_list rp/*)*/
	    {
	        chk_parm($2, parm_default($5));
	        all_parm($5);
	        $$ = $2;
	    } 
	| VOID Identifier '(' 
	    {
	        make_func($2, 0);
	        blk_push();
	    }
	  optional_parameter_list rp/*)*/
	    {
	        chk_parm($2, parm_default($5));
	        all_parm($5);
	        $$ = $2;
	    } 

optional_parameter_list
	: /* no formal parameters */
	    {
	        $$ = 0;
	    }
	| parameter_declaration_list
	    /* $$ = $1 = chain of formal parameters */

parameter_declaration_list
	: parameter_declaration
        {
            $$ = link_parm($1, 0);
        }
	| parameter_declaration_list ',' parameter_declaration
        {
            $$ = link_parm($3, $1);
            yyerrok;
        }
    | error
        {
            $$ = 0;
        }
    | error parameter_declaration_list
        {
            $$ = $2;
        }
	| parameter_declaration error parameter_declaration_list
        {
            $$ = link_parm($1, $3);
        }
	| error ',' parameter_declaration_list
        {
            $$ = $3;
            yyerrok;
        }

parameter_declaration
	: type_specifier parameter_declarator
	    {
	        $$ = $<y_sym>2;
	    }

type_specifier
	: VOID
	| INT

parameter_declarator
	: pointer parameter_direct_declarator
	    {
	        $$ = $<y_sym>2;
	    }
	| parameter_direct_declarator
	    {
	        $$ = $<y_sym>1;
	    }

parameter_direct_declarator
    : Identifier
        {
            all_var($1, 0, 0);
        }
    | Identifier '[' rb/*']'*/
        {
            all_var($1, 0, 0);
        }
    
compound_statement
	: '{' 
	    {
	        $<y_lab>$ = l_offset;
	        blk_push();
	    }
	  declarations statements rr/*'}'*/
	    {
	        if (l_offset > l_max)
	            l_max = l_offset;
	        l_offset = $<y_lab>2;
	        blk_pop();
	    }

declarations
	: /* null */
	| declarations declaration
	    {
	        yyerrok;
	    }
	| declarations error

declaration
	: INT declarator_list sc/*';'*/
	| function_declaration sc/*';'*/
	    {
	        blk_pop();
	    }

declarator_list
	: declarator
	| declarator_list ',' declarator
	    {
	        yyerrok;
	    }
	| error
	| declarator_list error
	| declarator_list error declarator
	    {
	        yyerrok;
	    }
	| declarator_list ',' error

declarator
	: direct_declarator
	| pointer_declarator

direct_declarator
    : Identifier
        {
            all_var($1, 0, 0);
        }
    | Identifier '=' initializer
        {
            all_var($1, 0, 0);
	        gen_direct(OP_STORE, gen_mod($1), OFFSET($1), NAME($1));
	        //gen_pr(OP_POP, "clear stack for assignment");
        }
    | Identifier '[' Constant rb
        {
            all_var($1, atoi($3), 0);
        }

pointer_declarator
    : pointer Identifier
        {
            all_var($2, 0, $1);
        }
    | pointer Identifier '=' initializer
        {
            all_var($2, 0, $1);
	        gen_direct(OP_STORE, gen_mod($2), OFFSET($2), NAME($2));
	        //gen_pr(OP_POP, "clear stack pointer assignment");
        }

pointer
    : '*'
        {
            $<y_num>$ = 1;
        }
    | '*' pointer	
        {
            $<y_num>$++;
        }
    
initializer
    : Constant
	    {
	        gen_load_immed($1);
	    }
    
statements
	: /* null */
	| statements statement
	    {
	        yyerrok;
	    }
	| statements error

statement
	: expression sc/*';'*/
	    {
	        if (!is_void_expr)
	        {
	            //gen_pr(OP_POP, "clear stack for expression");
	        }
	        else
	        {
	            is_void_expr = 0;
	        }
	    }
	| sc/*';'*/  /* null statement */
	| BREAK sc/*';'*/
	    {gen_break();}
	| CONTINUE sc/*';'*/
	    {gen_continue();}
	| RETURN sc/*';'*/
	    //{gen_pr(OP_RETURN, "RETURN from void fct");}
	| RETURN expression sc/*';'*/
	    {
	        gen_pop(OP_POP_RET, "value to return");
	        //gen_pr(OP_RETURN, "RETURN value from fct");
	    }
	| compound_statement
	| if_prefix statement
	    {gen_label($1);}
	| if_prefix statement ELSE 
	    {
	        $<y_lab>$ = gen_jump(OP_JUMP, new_label(), "past ELSE");
	        gen_label($1);
	    }
	  statement
	    {gen_label($<y_lab>4);}
	| loop_prefix 
	    {
	        $<y_lab>$ = gen_jump(OP_JUMPZ, new_label(), "WHILE");
	        push_break($<y_lab>$);
	    }
	  statement
	    {
	        gen_jump(OP_JUMP, $1, "repeat WHILE");
	        gen_label($<y_lab>2);
	        pop_break();
	        pop_continue();
	    }

if_prefix
	: IF '(' expression rp/*')'*/
	    {$$ = gen_jump(OP_JUMPZ, new_label(), "IF");}
	| IF error
	    {$$ = gen_jump(OP_JUMPZ, new_label(), "IF");}

loop_prefix
	: WHILE '(' 
	    {
	        $<y_lab>$ = gen_label(new_label());
	        push_continue($<y_lab>$);
	    }
	  expression rp/*')'*/
	    {$$ = $<y_lab>3;}
	| WHILE error
	    {
	        $$ = gen_label(new_label());
	        push_continue($$);
	    }

expression
	: binary
	| expression ','
	    {
	        yyerrok;
	        gen_pop(OP_POP_ARG, "???discard???");
	    }
	  binary
	| error ',' binary
	    {
	        yyerrok;
	    }
	| expression error
	| expression ',' error

binary
    : Identifier
	    { 
	        if (chk_var($1) == 0)
	        {
	            // scalar
	            gen_direct(OP_LOAD, gen_mod($1), OFFSET($1), NAME($1));
	        }
	        else
	        {
	            // array
	            char comment[80];
	            sprintf(comment, "&%s",  NAME($1));
	            gen_reference(OP_LOAD, gen_mod($1), OFFSET($1), comment);
	        }
	    }
	| Constant
	    {
	        gen_load_immed($1);
	    }
	| '(' expression rp/*')'*/
	    {$$ = $<y_sym>1;}
	| '(' error rp/*')'*/
	    {$$ = $<y_sym>1;}
	| Identifier '(' rp/*')'*/
	    {
	        gen_call($1, 0);
	        if (TYPE($1) == VFUNC)
	        {
	            is_void_expr = 1;
	        }
	    }
	| Identifier '(' 
	    {
	        chk_func($1);
	    }
	  argument_list rp/*')'*/
	    {
	        gen_call($1, $4);
	        if (TYPE($1) == VFUNC)
	        {
	            is_void_expr = 1;
	        }
	    }
	| Identifier '[' binary rb/*']'*/
	    {
	        // array as a RHS
	        char name[80];
	        sprintf(name, "%s[]",  NAME($1));
	        chk_var($1);
	        gen_indirect(OP_LOAD, gen_mod($1), OFFSET($1), name, 1);
	    }
	| '&' Identifier
	    {
	        // address op as a RHS
	        char name[80];
	        sprintf(name, "&%s",  NAME($2));
	        chk_var($2);
	        gen_reference(OP_LOAD, gen_mod($2), OFFSET($2), name);
	    }
	| '*' Identifier
	    {
	        // indirection op as a RHS
	        char name[80];
	        sprintf(name, "*%s",  NAME($2));
	        chk_var($2);
	        gen_pointer(OP_LOAD, gen_mod($2), OFFSET($2), name, 1);
	    }
	| PP Identifier
	    {
	        chk_var($2);
	        gen_direct(OP_INC, gen_mod($2), OFFSET($2), NAME($2));
	    }
	| MM Identifier
	    {
	        chk_var($2);
	        gen_direct(OP_DEC, gen_mod($2), OFFSET($2), NAME($2));
	    }
	| Identifier PP
	    {
	        chk_var($1);
	        gen_direct(OP_POST_INC, gen_mod($1), OFFSET($1), NAME($1));
	    }
	| Identifier MM
	    {
	        chk_var($1);
	        gen_direct(OP_POST_DEC, gen_mod($1), OFFSET($1), NAME($1));
	    }
	| '!' binary
	    {
	        gen_alu(ALU_NOT, "!");
	    }
	| '~' binary
	    {
	        gen_alu(ALU_INV, "~");
	    }
	| '-' binary
	    {
	        gen_alu(ALU_NEG, "-");
	    }
	| binary '+' binary
	    {
	        gen_alu(ALU_ADD, "+");
	    }
	| binary '-' binary
	    {
	        gen_alu(ALU_SUB, "-");
	    }
	| binary '*' binary
	    {
	        gen_alu(ALU_MUL, "*");
	    }
	| binary '/' binary
	    {
	        gen_alu(ALU_DIV, "/");
	    }
	| binary '%' binary
	    {
	        gen_alu(ALU_MOD, "%");
	    }
	| binary '>' binary
	    {
	        gen_alu(ALU_GT, ">");
	    }
	| binary '<' binary
	    {
	        gen_alu(ALU_LT, "<");
	    }
	| binary GE binary
	    {
	        gen_alu(ALU_GE, ">=");
	    }
	| binary LE binary
	    {
	        gen_alu(ALU_LE, "<=");
	    }
	| binary EQ binary
	    {
	        gen_alu(ALU_EQ, "==");
	    }
	| binary NE binary
	    {
	        gen_alu(ALU_NE, "!=");
	    }
	| binary '&' binary
	    {
	        gen_alu(ALU_AND, "&");
	    }
	| binary '|' binary
	    {
	        gen_alu(ALU_OR, "|");
	    }
	| binary '^' binary
	    {
	        gen_alu(ALU_XOR, "^");
	    }
	| binary AND binary
	    {
	        gen_alu(ALU_LAND, "&&");
	    }
	| binary OR binary
	    {
	        gen_alu(ALU_LOR, "||");
	    }
	| Identifier '=' binary
	    { 
	        chk_var($1); 
	        gen_direct(OP_STORE, gen_mod($1), OFFSET($1), NAME($1));
	    }
	| array_lhs_spec '=' binary
	    {
	        // array as a LHS 
	        char name[80];
	        sprintf(name, "%s[]",  NAME($1));
	        chk_var($1); 
	        gen_indirect(OP_STORE, gen_mod($1), OFFSET($1), name, 0);
	    }
	| pointer_lhs_spec '=' binary
	    {
	        // pointer as a LHS 
	        char name[80];
	        sprintf(name, "*%s",  NAME($1));
	        chk_var($1); 
	        gen_pointer(OP_STORE, gen_mod($1), OFFSET($1), name, 0);
	    }
	| Identifier PE
	    { 
	        chk_var($1); 
	        gen_direct(OP_LOAD, gen_mod($1), OFFSET($1), NAME($1));
	    }
	  binary
	    { 
	        gen_alu(ALU_ADD, "+");
	        gen_direct(OP_STORE, gen_mod($1), OFFSET($1), NAME($1));
	    }
	| Identifier ME
	    { 
	        chk_var($1); 
	        gen_direct(OP_LOAD, gen_mod($1), OFFSET($1), NAME($1));
	    }
	  binary
	    { 
	        gen_alu(ALU_SUB, "-");
	        gen_direct(OP_STORE, gen_mod($1), OFFSET($1), NAME($1));
	    }
	| Identifier TE
	    { 
	        chk_var($1); 
	        gen_direct(OP_LOAD, gen_mod($1), OFFSET($1), NAME($1));
	    }
	  binary
	    { 
	        gen_alu(ALU_MUL, "*");
	        gen_direct(OP_STORE, gen_mod($1), OFFSET($1), NAME($1));
	    }
	| Identifier DE
	    { 
	        chk_var($1); 
	        gen_direct(OP_LOAD, gen_mod($1), OFFSET($1), NAME($1));
	    }
	  binary
	    { 
	        gen_alu(ALU_DIV, "/");
	        gen_direct(OP_STORE, gen_mod($1), OFFSET($1), NAME($1));
	    }
	| Identifier RE
	    { 
	        chk_var($1); 
	        gen_direct(OP_LOAD, gen_mod($1), OFFSET($1), NAME($1));
	    }
	  binary
	    { 
	        gen_alu(ALU_MOD, "%");
	        gen_direct(OP_STORE, gen_mod($1), OFFSET($1), NAME($1));
	    }
	| Identifier AE
	    { 
	        chk_var($1); 
	        gen_direct(OP_LOAD, gen_mod($1), OFFSET($1), NAME($1));
	    }
	  binary
	    { 
	        gen_alu(ALU_AND, "&");
	        gen_direct(OP_STORE, gen_mod($1), OFFSET($1), NAME($1));
	    }
	| Identifier OE
	    { 
	        chk_var($1); 
	        gen_direct(OP_LOAD, gen_mod($1), OFFSET($1), NAME($1));
	    }
	  binary
	    { 
	        gen_alu(ALU_OR, "|");
	        gen_direct(OP_STORE, gen_mod($1), OFFSET($1), NAME($1));
	    }

argument_list
	: binary
	    {$$ = 1;}
	| argument_list ',' binary
	    {
	        ++$$;
	        yyerrok;
	    }
	| error
	    {$$ = 0;}
	| argument_list error
	| argument_list ',' error

array_lhs_spec
    : Identifier '[' binary ']'
        {
	        char name[80];
            sprintf(name, "%s[]",  NAME($1));
	        chk_var($1); 
            gen_indirect(OP_LOAD, gen_mod($1), OFFSET($1), name, 0);
	    }

pointer_lhs_spec
	: '*' Identifier
	    {
	        $$ = $2;
	        char name[80];
	        sprintf(name, "*%s",  NAME($2));
	        chk_var($2); 
	        gen_pointer(OP_LOAD, gen_mod($2), OFFSET($2), name, 0);
	    }
/*
 *  make certain terminal symbols very important
 */

rp  : ')'   {yyerrok;}	    
sc  : ';'   {yyerrok;}	    
rr  : '}'   {yyerrok;}	    
rb  : ']'   {yyerrok;}	    

