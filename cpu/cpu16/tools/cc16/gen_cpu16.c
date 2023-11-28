/*
 *  gen.c - hack code generator
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
#include "symtab.h"
#include "gen.h"

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
	//fprintf(yyout, "    jmp     __Begin\n\n");
	fprintf(yyout, "    jmp     main\n\n");
	fprintf(yyout, "; define all register definitions and return codes\n");
	fprintf(yyout, "#include <asm16/system16.asm>\n\n");
	fprintf(yyout, "; insert all libasm code\n");
	fprintf(yyout, "#include <asm16/libasm.asm>\n\n");
	//fprintf(yyout, "; this is the true beginning of the app (global code)\n");
	//fprintf(yyout, "__Begin:\n");
}

void GenEndProg()
{
    fprintf(yyout, "\n; end program\n");

    fprintf(yyout, "__Exit:\n");
    fprintf(yyout, "    bra    __Exit\n\n");
}

void GenCall(const char *fctname)
{
    fprintf(yyout, "\n; call\n");

    fprintf(yyout, "    jsr     %s\n", fctname);        // call function
}

void GenEntry(const char *fctname, const char *symbol)
{    
    fprintf(yyout, "\n; fct entry\n");

    curFctName = (char *)fctname;    
    fprintf(yyout, "%s:\n", fctname);
    fprintf(yyout, "    push    bp\n");                // push current BP
    fprintf(yyout, "    mov     bp,sp\n");             // move SP to BP  
    fprintf(yyout, "    sub     sp,#%s\n", symbol);    // adjust SP past local variables
}

void GenAlu(const char *mod, const char *comment)
{
    fprintf(yyout, "\n; alu, %s, %s\n", mod, comment);
    
	if (!strcmp(mod, "+") || !strcmp(mod, "-") || !strcmp(mod, "&") || !strcmp(mod, "|") || !strcmp(mod, "&&") || !strcmp(mod, "||") || !strcmp(mod, "^"))
	{
		fprintf(yyout, "    pop    bx\n");          // pop y
		fprintf(yyout, "    pop    ax\n");          // pop x
		if (!strcmp(mod, "+"))
		{
			fprintf(yyout, "    add    ax,bx\n");   // TOS = x + y
		}
		else if (!strcmp(mod, "-"))
		{
			fprintf(yyout, "    sub    ax,bx\n");   // TOS = x - y
		}
		else if (!strcmp(mod, "&") || !strcmp(mod, "&&"))
		{
			fprintf(yyout, "    and    ax,bx\n");   // TOS = x & y
		}
		else if (!strcmp(mod, "|") || !strcmp(mod, "||"))
		{
			fprintf(yyout, "    or     ax,bx\n");    // TOS = x | y
		}
		else if (!strcmp(mod, "^"))
		{
			fprintf(yyout, "    xor    ax,bx\n");    // TOS = x | y
		}
		fprintf(yyout, "    push    ax\n");         // push x
	}
	// TODO: create Multiply and Divide subroutines in the stdlib
	else if (!strcmp(mod, "*") || !strcmp(mod, "/"))
	{
		if (!strcmp(mod, "*"))
		{
		    // create a dummy symbol table entry for Multiply() fct
			GenCall("Multiply");       // TOS = x * y
		}
		else if (!strcmp(mod, "/"))
		{
		    // create a dummy symbol table entry for Divide() fct
			GenCall("Divide");	    // TOS = x / y
		}
	    fprintf(yyout, "    @RETVAL\n");
	    fprintf(yyout, "    D=M\n");
	    fprintf(yyout, "    @SP\n");
	    fprintf(yyout, "    M=M-1\n");
	    fprintf(yyout, "    A=M\n");
	    fprintf(yyout, "    M=D\n");
    }
	else if (!strcmp(mod, "!"))
	{
	    // replace TOS with its logical inverse
        // FIXME: asm16 can't deal with negative numbers
		//fprintf(yyout, "    mov     bx,#-1\n");               // check for TOS==0, if so set TOS=-1, else TOS=0
		fprintf(yyout, "    mov     bx,@0xffff\n");             // check for TOS==0, if so set TOS=-1, else TOS=0
		fprintf(yyout, "    pop     ax\n");                     // check for 0 (x ^ 0 = 0 iff x is 0)
		fprintf(yyout, "    xor     ax,#0\n");
		fprintf(yyout, "    bz      LT%d\n", ++labelId);        // if x is not 0 make it 0
		fprintf(yyout, "    zero    bx\n");
		fprintf(yyout, "LT%d:\n", labelId);
        fprintf(yyout, "    mov     ax,bx\n");
        fprintf(yyout, "    push    ax\n");
	}
	else if (!strcmp(mod, "~"))
	{
	    // replace TOS with its bitwise inverse
        fprintf(yyout, "    pop     ax\n");                     // ~x := x ^ -1
        // FIXME: asm16 can't deal with negative numbers
        //fprintf(yyout, "    xor     ax,#-1\n");
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
	else if (!strcmp(mod, "==") || !strcmp(mod, "!=") || !strcmp(mod, ">") || !strcmp(mod, ">=") || 
	         !strcmp(mod, "<") || !strcmp(mod, "<="))
	{
		fprintf(yyout, "    mov     cx,@0xffff\n");             // assume result is true
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
		fprintf(yyout, "    mov     cx,#0\n");                  // truth is disproven so make result false
		fprintf(yyout, "LT%d:\n", labelId);
        fprintf(yyout, "    push    cx\n");
	}
	else
	{
		fprintf(yyout, "illegal mod\n");
	}
}

void GenLoadImmed(const char *constant)
{
    fprintf(yyout, "\n; load immed\n");

    fprintf(yyout, "    mov     ax,@%s\t; constant\n", constant);     // push constant
    fprintf(yyout, "    push    ax\n");
}

// set the effective address (M) based on the variable type
static void GenAccessVar(const char *vartype, int offset, const char *globalName)
{
    if (!strcmp(vartype, "gbl"))
    {
        // globals reside in lowest memory at offset from zero
        // TODO: once this works in general change to ZP addressing mode
        fprintf(yyout, "    mov     bx,#0x%02x\t; global %s\n", offset, globalName); // simply use offset (from 0x0000 in RAM)
    }
    else if (!strcmp(vartype, "par"))
    {
        // parameter n is at BP+n+3
        fprintf(yyout, "    mov     bx,bp\t; param %s\n", globalName); // adjust the var offset from the BP
        fprintf(yyout, "    add     bx,#%d\n", offset + 3);
    }
    else if (!strcmp(vartype, "lcl"))
    {
        // local variable n is at BP-n (n is offset)
        fprintf(yyout, "    mov     bx,bp\t; local %s\n", globalName); // address of var is BP-offset
        fprintf(yyout, "    sub     bx,#%d\n", offset);
    }
    else if (!strcmp(vartype, "fct"))
    {
        fprintf(yyout, "    push    ax\t; %s\n", globalName); // function retval is in ax
    }
    else
    {
        fprintf(stderr, "Error: illegal variable type - exiting...\n");
        exit(1);
    }
}

void GenDirect(const char *op, const char *vartype, int offset, const char *globalName)
{
    if (!strcmp(op, OP_LOAD))
    {
        fprintf(yyout, "\n; load direct\n");
        
        GenAccessVar(vartype, offset, globalName);      // point to the effective address of the var

        // only load the value if it's not a function return value
        if (strcmp(vartype, "fct") != 0)
        {
            fprintf(yyout, "    mov     ax,[bx]\n");    // push the var
            fprintf(yyout, "    push    ax\n");
        }
    }
    else if (!strcmp(op, OP_STORE))
    {
        fprintf(yyout, "\n; store direct\n");

        GenAccessVar(vartype, offset, globalName);      // get the var _address_
        fprintf(yyout, "    pop     ax\n");             // store the value to the variable
        fprintf(yyout, "    mov     [bx],ax\n");
    }
    // FIXME: all pre/post inc/dec push a value onto the stack so a stand-alone
    //        version will not have a matching pop causing the stack to grow - 
    //        this can only be fixed by altering the grammar
    else if (!strcmp(op, OP_INC))
    {
        fprintf(yyout, "\n; pre-inc\n");
        
        GenAccessVar(vartype, offset, globalName);
        fprintf(yyout, "    mov     ax,[bx]\n");        // increment then push the var
        fprintf(yyout, "    inc     ax\n");
        fprintf(yyout, "    mov     [bx],ax\n");
        fprintf(yyout, "    push    ax\n");
    }
    else if (!strcmp(op, OP_POST_INC))
    {
        fprintf(yyout, "\n; post-inc\n");
        
        GenAccessVar(vartype, offset, globalName);
        fprintf(yyout, "    mov     ax,[bx]\n");        // push then increment the var
        fprintf(yyout, "    push    ax\n");             
        fprintf(yyout, "    inc     ax\n");
        fprintf(yyout, "    mov     [bx],ax\n");
    }
    else if (!strcmp(op, OP_DEC))
    {
        fprintf(yyout, "\n; pre-dec\n");
       
        GenAccessVar(vartype, offset, globalName);
        fprintf(yyout, "    mov     ax,[bx]\n");        // decrement then push the var
        fprintf(yyout, "    dec     ax\n");
        fprintf(yyout, "    mov     [bx],ax\n");
        fprintf(yyout, "    push    ax\n");
    }
    else if (!strcmp(op, OP_POST_DEC))
    {
        fprintf(yyout, "\n; post-dec\n");
       
        GenAccessVar(vartype, offset, globalName);
        fprintf(yyout, "    mov     ax,[bx]\n");        // push then decrement the var
        fprintf(yyout, "    push    ax\n");             
        fprintf(yyout, "    dec     ax\n");
        fprintf(yyout, "    mov     [bx],ax\n");
    }
}

void GenIndirect(const char *op, const char *vartype, int offset, const char *globalName, int is_rhs)
{
    if (!strcmp(op, OP_LOAD))
    {
        fprintf(yyout, "\n; load indirect\n");

        // replace the index at TOS with the base address - index (stack grows downward)   
        GenAccessVar(vartype, offset, globalName);      // get the base address
        if (!strcmp(vartype, "par"))
        {
            fprintf(yyout, "    mov     bx,[bx]\n");    // deref the base address pointer for array params
        }
        fprintf(yyout, "    pop     ax\n");             // subtract the index from the base address to get the EA
        fprintf(yyout, "    sub     bx,ax\n");
        if (!is_rhs)
        {
            fprintf(yyout, "    push    bx\n");         // for LHS simply push the EA
        }
        else
        {
            fprintf(yyout, "    mov     ax,[bx]\n");    // for RHS dereference the EA to get the actual value
            fprintf(yyout, "    push    ax\n");         // then push the value
        }
    }
    else if (!strcmp(op, OP_STORE))
    {
        fprintf(yyout, "\n; store indirect\n");

        // store the value at TOS to the address at TOS-1
        fprintf(yyout, "    pop     ax\n");             // pop the value
        fprintf(yyout, "    pop     bx\n");             // pop the address
        fprintf(yyout, "    mov     [bx],ax\n");        // store the value at the address
    }
}

void GenPointer(const char *op, const char *vartype, int offset, const char *globalName, int is_rhs)
{
    if (!strcmp(op, OP_LOAD))
    {
        fprintf(yyout, "\n; load pointer\n");

        // push the contents of the pointer
        GenAccessVar(vartype, offset, globalName);
        if (is_rhs)
        {
            fprintf(yyout, "    mov     bx,[bx]\n");    // dereference the EA to get the actual value
        }
        fprintf(yyout, "    mov     ax,[bx]\n");        // push the var
        fprintf(yyout, "    push    ax\n");
    }
    else if (!strcmp(op, OP_STORE))
    {
        fprintf(yyout, "\n; store pointer\n");

        fprintf(yyout, "    pop    ax\n");              // both the value and the pointer were pushed so pop the value then
        fprintf(yyout, "    pop    bx\n");              // pop the pointer and move the value indirectly thru the pointer
        fprintf(yyout, "    mov    [bx],ax\n");
    }
}

void GenReference(const char *op, const char *vartype, int offset, const char *globalName)
{
    if (!strcmp(op, OP_LOAD))
    {
        fprintf(yyout, "\n; load reference\n");
                     
        GenAccessVar(vartype, offset, globalName);      // get the base address
        fprintf(yyout, "    push    bx\n");             // simply push it, no need to dereference
    }
}

void GenPop(const char *op, const char *comment)
{
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

void GenEqu(const char *symbol, int value)
{
    fprintf(yyout, ".define %s %d\n", symbol, value);
}

// end of gen_cpu16.c

