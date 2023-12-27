/*
 *  gen_cpu16.c
 *
 *  Description: This is the CPU-specific code generator for cc16. The functions
 *  in this module are called from the cc16 abstract code generator.  The functions
 *  performs code generation for the following C code elements:
 *      - immediate values
 *      - direct, i.e. scaler, values
 *      - arrays
 *      - pointers
 *      - references
 *      - functions
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
#include "symtab.h"
#include "gen.h"
#include "gen_cpu16.h"

// output file as defined in the parser
extern FILE *yyout;

char *curFileName;
char *curFctName;
int labelId = 0;
int returnId = 0;
unsigned curLocalVarQty;

#define STACK_BASE 0x0fff
#define FRAME_BASE 0
// the global base is implicitly 0

void GenBeginProg()
{
    fprintf(yyout, "\n; begin program\n");
    
	// set the initial stack base and frame pointer
	fprintf(yyout, "; initialize the runtime\n");
	fprintf(yyout, "    mov     sp,@0x%04x\n", STACK_BASE);
	fprintf(yyout, "    zero    bp\n");
	fprintf(yyout, "    jmp     main\n\n");
	fprintf(yyout, "; define all register definitions and return codes\n");
	fprintf(yyout, "#include <system16/system16.asm>\n\n");
	fprintf(yyout, "; insert all libasm code\n");
	fprintf(yyout, "#include <system16/libasm.asm>\n\n");
}

void GenEndProg()
{
    fprintf(yyout, "\n; end program\n");

    fprintf(yyout, "__Exit:\n");
    fprintf(yyout, "    bra    __Exit\n\n");
}

// create the stack frame: push current BP, move SP to BP, then adjust SP past local variables
void GenEntry(const char *fctname, const char *symbol)
{    
    fprintf(yyout, "\n; fct entry\n");

    curFctName = (char *)fctname;    
    fprintf(yyout, "%s:\n", fctname);
    fprintf(yyout, "    push    bp\n");
    fprintf(yyout, "    mov     bp,sp\n");
    fprintf(yyout, "    sub     sp,@%s\n", symbol);
}

// C operator code generation
void GenAlu(const char *mod, const char *comment)
{
    fprintf(yyout, "\n; operator %s\n", comment);

    // FIXME: break out ALU generation into binary and unary operators
    // binary operators    
	if (
	    !strcmp(mod, "+") || !strcmp(mod, "-") || // !strcmp(mod, "++") || !strcmp(mod, "--")
	    !strcmp(mod, "&&") || !strcmp(mod, "||") ||
	    !strcmp(mod, "&") || !strcmp(mod, "|")  || !strcmp(mod, "^") || !strcmp(mod, "<<") || !strcmp(mod, ">>")
	)
	{
		fprintf(yyout, "    pop     bx\n");                      // pop x and y
		fprintf(yyout, "    pop     ax\n");
		
		// arithmetic operators
		if (     !strcmp(mod, "+"))
		{
			fprintf(yyout, "    add     ax,bx\n");               // x + y
		}
		else if (!strcmp(mod, "-"))
		{
			fprintf(yyout, "    sub     ax,bx\n");               // x - y
		}
		
		// logical operators
		else if (!strcmp(mod, "&&"))
		{
			fprintf(yyout, "    zero    cx\n");                  // assume result is false
			fprintf(yyout, "    or      ax,#0\n");               // if ax is false exit
		    fprintf(yyout, "    bz      LT%d\n", ++labelId);
			fprintf(yyout, "    or      bx,#0\n");               // if bx is false exit
		    fprintf(yyout, "    bz      LT%d\n", labelId);
			fprintf(yyout, "    mov     cx,#1\n");               // result is true iff both are true
		    fprintf(yyout, "LT%d:\n", labelId);
			fprintf(yyout, "    mov     ax,cx\n");               // load ax with result
		}
		else if (!strcmp(mod, "||"))
		{
			fprintf(yyout, "    zero    cx\n");                  // assume result is false
			fprintf(yyout, "    or      ax,bx\n");               // result is false iff both are false so exit
		    fprintf(yyout, "    bz      LT%d\n", ++labelId);
			fprintf(yyout, "    mov     cx,#1\n");               // otherwise flag true
		    fprintf(yyout, "LT%d:\n", labelId);
			fprintf(yyout, "    mov     ax,cx\n");               // load ax with result
		}
				
		// bitwise operators
		else if (!strcmp(mod, "&"))
		{
			fprintf(yyout, "    and     ax,bx\n");               // x & y
		}
		
		else if (!strcmp(mod, "|"))
		{
			fprintf(yyout, "    or      ax,bx\n");               // x | y
		}
		else if (!strcmp(mod, "^"))
		{
			fprintf(yyout, "    xor     ax,bx\n");               // x ^ y
		}
		else if (!strcmp(mod, "<<"))
		{
		    fprintf(yyout, "LT%d:\n", ++labelId);               // x << y
			fprintf(yyout, "    asl     ax\n");
			fprintf(yyout, "    dec     bx\n");
		    fprintf(yyout, "    bnz     LT%d\n", labelId);
		}
		else if (!strcmp(mod, ">>"))
		{
		    fprintf(yyout, "LT%d:\n", ++labelId);               // x >> y
			fprintf(yyout, "    lsr     ax\n");
			fprintf(yyout, "    dec     bx\n");
		    fprintf(yyout, "    bnz     LT%d\n", labelId);
		}
		fprintf(yyout, "    push     ax\n");                     // push the result
	}
	else if (!strcmp(mod, "*") || !strcmp(mod, "/") || !strcmp(mod, "%"))
	{
	    // these are well-known functions that reside in libasm and must be called
	    // the same way the parser would do so
	    // NOTE: add any other non-intrinsic operator functions here the same way, e.g. mod (%)
		if (!strcmp(mod, "*"))
		{
            GenCall("_Multiply");                               // TOS = x * y
		}
		else if (!strcmp(mod, "/"))
		{
            GenCall("_Divide");                                 // TOS = x / y
		}
		else if (!strcmp(mod, "%"))
		{
            GenCall("_Modulo");                                 // TOS = x % y
		}
        GenPop(OP_POP_ARG, "discard argument");                 // adjust the stack
        GenPop(OP_POP_ARG, "discard argument");
        GenDirect(OP_LOAD, MOD_FCT, 0, "function retval");      // load the return value
    }
    
    // logical operator
	else if (!strcmp(mod, "!"))
	{
	    // replace TOS with its logical inverse
		fprintf(yyout, "    mov     bx,#1\n");                  // assume result is true
		fprintf(yyout, "    pop     ax\n");                     // check for 0 (x ^ 0 = 0 iff x is 0)
		fprintf(yyout, "    xor     ax,#0\n");
		fprintf(yyout, "    bz      LT%d\n", ++labelId);        // if x is not 0 make it 0
		fprintf(yyout, "    zero    bx\n");             
		fprintf(yyout, "LT%d:\n", labelId);
        fprintf(yyout, "    mov     ax,bx\n");
        fprintf(yyout, "    push    ax\n");
	}
	
	// bitwise operator
	else if (!strcmp(mod, "~"))
	{
	    // replace TOS with its bitwise complement
        fprintf(yyout, "    pop     ax\n");                     // ~x := x ^ 0xffff
        fprintf(yyout, "    xor     ax,@0xffff\n");
        fprintf(yyout, "    push    ax\n");
	}
	else if (!strcmp(mod, ALU_NEG)) // "-"
	{
	    // replace TOS with its negative value
        fprintf(yyout, "    zero    ax\n");                     // -x := 0 - x
        fprintf(yyout, "    pop     bx\n");
        fprintf(yyout, "    sub     ax,bx\n");
        fprintf(yyout, "    push    ax\n");
		
	}
	
	// relational operators
	else if (!strcmp(mod, "==") || !strcmp(mod, "!=") || !strcmp(mod, ">") || !strcmp(mod, ">=") || 
	         !strcmp(mod, "<") || !strcmp(mod, "<="))
	{
		fprintf(yyout, "    mov     cx,#1\n");                  // assume result is true
		fprintf(yyout, "    pop     bx\n");                     // pop y
		fprintf(yyout, "    pop     ax\n");                     // pop x
		fprintf(yyout, "    sub     ax,bx\n");                  // make the logical comparison (x-y)
		if (!strcmp(mod, "=="))
		{
		    fprintf(yyout, "    bz      LT%d\n", ++labelId);    // x == y
		}
		else if (!strcmp(mod, "!="))
		{
		    fprintf(yyout, "    bnz     LT%d\n", ++labelId);    // x != y
		}
		else if (!strcmp(mod, ">"))
		{
		    fprintf(yyout, "    bz      LZ%d\n", ++labelId);    // x != y && x >= 0
		    fprintf(yyout, "    bpl     LT%d\n", labelId);
		    fprintf(yyout, "LZ%d:\n", labelId);
		}
		else if (!strcmp(mod, ">="))
		{
		    fprintf(yyout, "    bpl     LT%d\n", ++labelId);    // x >= y
		}
		else if (!strcmp(mod, "<"))
		{
		    fprintf(yyout, "    bmi     LT%d\n", ++labelId);    // x < y
		}
		else if (!strcmp(mod, "<="))
		{
		    fprintf(yyout, "    bmi     LT%d\n", ++labelId);    // x < y || x == y
		    fprintf(yyout, "    bz      LT%d\n", labelId);
		}
		fprintf(yyout, "    zero    cx\n");                     // truth is disproven so make result false
		fprintf(yyout, "LT%d:\n", labelId);
        fprintf(yyout, "    push    cx\n");
	}
	else
	{
		fprintf(yyout, "illegal operator\n");
	}
}

// load the address of a variable into BX based on its type
static void GenAccessVar(const char *vartype, int offset, const char *globalName)
{
    if (!strcmp(vartype, "gbl"))
    {
        // globals reside in lowest memory at offset from zero so use direct address mode
        fprintf(yyout, "    mov     bx,@0x%02x\t; global %s\n", offset, globalName);
    }
    else if (!strcmp(vartype, "par"))
    {
        // parameter n is at BP+n+3
        fprintf(yyout, "    mov     bx,bp\t; param %s\n", globalName);
        fprintf(yyout, "    add     bx,@%d\n", offset + 3);
    }
    else if (!strcmp(vartype, "lcl"))
    {
        // local variable n is at BP-n (n is offset)
        fprintf(yyout, "    mov     bx,bp\t; local %s\n", globalName);
        fprintf(yyout, "    sub     bx,@%d\n", offset);
    }
}

// load a variable into AX based on its type
static void GenLoadVar(const char *vartype, int offset, const char *globalName)
{
    if (!strcmp(vartype, "gbl"))
    {
        // use ZP addressing with offset (0x00-0xff) for globals
        fprintf(yyout, "    mov     ax,[#0x%02x]\t; global %s\n", offset, globalName);
    }
    else if (!strcmp(vartype, "par"))
    {
        // parameter n is at BP+n+3 and can be accessed with a single 5-bit offset instruction
        fprintf(yyout, "    mov     ax,[bp+0x%x]\t; param %s\n", (offset+3) & 0x1f, globalName);
    }
    else if (!strcmp(vartype, "lcl"))
    {
        // local variable n is at BP-n (n is offset)
        fprintf(yyout, "    mov     ax,[bp+0x%x]\t; local %s\n", (-offset) & 0x1f, globalName);
    }
}

// store a value from AX based on its type
static void GenStoreVar(const char *vartype, int offset, const char *globalName)
{
    if (!strcmp(vartype, "gbl"))
    {
        // use ZP addressing with offset (0x00-0xff) for globals
        fprintf(yyout, "    mov     [#0x%02x],ax\t; global %s\n", offset, globalName);
    }
    else if (!strcmp(vartype, "par"))
    {
        // parameter n is at BP+n+3 and can be accessed with a single 5-bit offset instruction
        fprintf(yyout, "    mov     [bp+0x%x],ax\t; param %s\n", (offset+3) & 0x1f, globalName); // access the var offset from the BP
    }
    else if (!strcmp(vartype, "lcl"))
    {
        // local variable n is at BP-n (n is offset)
        fprintf(yyout, "    mov     [bp+0x%x],ax\t; local %s\n", (-offset) & 0x1f, globalName); // access the var offset from the BP
    }
}

void GenLoadImmed(const char *constant)
{
    fprintf(yyout, "\n; load immed\n");

    fprintf(yyout, "    mov     ax,@%s\n", constant);     // push constant
    fprintf(yyout, "    push    ax\n");
}

// direct variable code generation
void GenDirect(const char *op, const char *vartype, int offset, const char *globalName)
{
    if (!strcmp(op, OP_LOAD))
    {
        fprintf(yyout, "\n; load direct\n");
        
        // load a variable by type and push it (function retvals don't need to be loaded)
        GenLoadVar(vartype, offset, globalName);
        fprintf(yyout, "    push    ax\n");
    }
    else if (!strcmp(op, OP_STORE))
    {
        fprintf(yyout, "\n; store direct\n");

        // pop a variable and store it based on its type
        fprintf(yyout, "    pop     ax\n");
        GenStoreVar(vartype, offset, globalName);
    }
}

// array code generation
void GenIndirect(const char *op, const char *vartype, int offset, const char *globalName, int is_rhs)
{
    if (!strcmp(op, OP_LOAD))
    {
        fprintf(yyout, "\n; load indirect\n");

        // replace the array index at TOS with the base address - index (stack grows downward)   
        GenAccessVar(vartype, offset, globalName);
        if (!strcmp(vartype, "par"))
        {
            // deref the address for param variables
            fprintf(yyout, "    mov     bx,[bx]\n");
        }

        // subtract the index from the base address to get the EA
        fprintf(yyout, "    pop     ax\n");             
        fprintf(yyout, "    sub     bx,ax\n");
        if (is_rhs)
        {
            // for RHS dereference the EA to get the actual value
            fprintf(yyout, "    mov     bx,[bx]\n");
        }
        fprintf(yyout, "    push    bx\n");
    }
    else if (!strcmp(op, OP_STORE))
    {
        fprintf(yyout, "\n; store indirect\n");

        // simply pop the value first and then the address and store the value indirectly to the address
        fprintf(yyout, "    pop     ax\n");
        fprintf(yyout, "    pop     bx\n");
        fprintf(yyout, "    mov     [bx],ax\n");
    }
}

// pointer code generation
void GenPointer(const char *op, const char *vartype, int offset, const char *globalName, int is_rhs)
{
    if (!strcmp(op, OP_LOAD))
    {
        fprintf(yyout, "\n; load pointer\n");

        // load the variable by type then push it
        GenLoadVar(vartype, offset, globalName);
        if (is_rhs)
        {
            // dereference to get the actual value for RHS
            fprintf(yyout, "    mov     ax,[ax]\n");
        }
        fprintf(yyout, "    push    ax\n");
    }
    else if (!strcmp(op, OP_STORE))
    {
        fprintf(yyout, "\n; store pointer\n");
        
        // both the value and the pointer are on the stack so pop them and store the value indirectly thru the pointer
        fprintf(yyout, "    pop     ax\n");
        fprintf(yyout, "    pop     bx\n");
        fprintf(yyout, "    mov     [bx],ax\n");
    }
}

// reference (i.e. pointer dereference) code generation
void GenReference(const char *op, const char *vartype, int offset, const char *globalName)
{
    if (!strcmp(op, OP_LOAD))
    {
        fprintf(yyout, "\n; load reference\n");

        // get the address of the variable by type then push it, no need to dereference
        GenAccessVar(vartype, offset, globalName);
        fprintf(yyout, "    push    bx\n");
    }
}

// function code generation
void GenCall(const char *fctname)
{
    fprintf(yyout, "\n; call\n");

    fprintf(yyout, "    jsr     %s\n", fctname);        // call function
}
void GenPop(const char *op, const char *comment)
{
    fprintf(yyout, "\n; pop\n");
        
    if (!strcmp(op, OP_POP_RET))
    {
	    fprintf(yyout, "    pop     ax ; %s\n", comment);
    }
    else if (!strcmp(op, OP_POP_ARG))
    {
	    fprintf(yyout, "    pop     bx ; %s\n", comment);
    }
}    
void GenReturn(const char *op, const char *comment)
{
    if (!strcmp(op, OP_RETURN))
    {
        fprintf(yyout, "\n; return\n");
        
        if (!strcmp(curFctName, "main"))
        {
            // this is unnecessary but included to easily ensure that the stack is cleaned up by the end of main
            fprintf(yyout, "    mov     sp,bp\n");      // move BP to SP to remove local vars
            fprintf(yyout, "    pop     bp\n");         // restore old BP
            fprintf(yyout, "    jmp    __Exit\n");      // for main() jump to the end of the program -- essentially "exit"
        }
        else
        {
            fprintf(yyout, "    mov     sp,bp\n");      // move BP to SP to remove local vars
            fprintf(yyout, "    pop     bp\n");         // restore old BP
            fprintf(yyout, "    rts\n");                // return from subroutine
        }
    }
}
void GenJump(const char *op, const char *label, const char *comment)
{
    if (!strcmp(op, OP_JUMPZ))
    {
        fprintf(yyout, "\n; jumpz\t%s\n", comment);
        
        fprintf(yyout, "    pop     ax\n");             // pop the logical value and jump if it's a false, i.e. zero
        fprintf(yyout, "    or      ax,#0\n");
        fprintf(yyout, "    bz      %s\n", label);
    }
    else
    {
        fprintf(yyout, "\n; jump\t%s\n", comment);
        
        fprintf(yyout, "    bra     %s\n", label);      // explicit jump
    }
}
void GenLabel(const char *label)
{
    fprintf(yyout, "%s:\n", label);
}

// value definition code generation
void GenEqu(const char *symbol, int value)
{
    fprintf(yyout, ".define %s %d\n", symbol, value);
}

// end of gen_cpu16.c

