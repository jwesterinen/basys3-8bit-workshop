;   modulo.asm
;
;   Subroutine: _Modulo
;
;   Description: This function performs the modulo, i.e. "remainder" operation.
;
;   Synopsis: int _Modulo(int dividend, int divisor);
;
;   Args:
;       a:  dividend
;       b:  divisor
;
;   Return: remainder of dividend / divisor
;
;   Stack on entry:
;     SP-> 
;          retaddr
;          divisor  (SP+2)
;          dividend (SP+3)
;
;   Regs used: ax, bx, cx, dx
;
;   Algorighm used:
;       mod(a,b) = (a - (b * (a / b)))


_Modulo:
    push    bp          ; setup the stack frame
    mov     bp,sp 
    push    bx          ; stash the context
    push    cx
    push    dx

    mov     bx,[bp+0x4]	; param a in bx
    mov     cx,[bp+0x3]	; param b in cx

    push    bx          ; call _Divide(bx,cx)
    push    cx
    jsr     _Divide
    pop     dx
    pop     dx          ; ax = (a / b)
    
    push    cx          ; call _Multiply(cx,ax)
    push    ax
    jsr     _Multiply
    pop     dx
    pop     dx          ; ax = (b * (a / b))
    
    sub     bx,ax       ; subtract
    mov     ax,bx       ; ax = (a - (b * (a / b)))

_Modulo_Return:
    pop     dx          ; restore the context
    pop     cx
    pop     bx
    mov     sp,bp       ; return from the subroutine
    pop     bp
    rts

