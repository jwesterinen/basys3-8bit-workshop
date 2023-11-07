;   expr_stack.asm
;
;   This file contains the implementation of an expression stack.  It contains
;   the following subroutines:
;
;    - _ExprPush:    push the value in ax onto the expression stack    
;    - _ExprPop:     pop the top of the expression stack into ax - if EP is 0, return 0    
;    - _ExprTop:     copy the top of the expression stack into ax
;    - _ExprPut:     copy ax into the top of the expression stack    

#ifndef EXPR_STACK_ASM
#define EXPR_STACK_ASM

.define exprStack 20;

_ExprPush:
    push    bx
    inc     ep
    mov     bx,@exprStack
    add     bx,ep
    mov     [bx],ax
    pop     bx
    rts
    
_ExprPop:
    zero    ax
    or      ep,#0
    bz      _ExprPop_End
    push    bx
    mov     bx,@exprStack
    add     bx,ep
    mov     ax,[bx]
    dec     ep
    pop     bx
_ExprPop_End:
    rts
    
_ExprTop:
    push    bx
    mov     bx,@exprStack
    add     bx,ep
    mov     ax,[bx]
    pop     bx
    rts

_ExprPut:
    push    bx
    mov     bx,@exprStack
    add     bx,ep
    mov     [bx],ax
    pop     bx
    rts
    
#endif // EXPR_STACK_ASM

