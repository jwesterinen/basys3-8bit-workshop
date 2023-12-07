;   multiply.asm
;
;   Subroutine: _Multiply
;
;   Description: This function performs a 16-bit multiplication.
;
;   Synopsis: int _Multiply(int multiplier, int multiplicand);
;
;   Args:
;       multiplier:   number of times multiplicand is added to itself
;       multiplicand: number to add to itself multiplier times
;
;   Return: product of the multiplier and multiplicand
;
;   Stack on entry:
;     SP-> 
;          retaddr
;          multiplicand (SP+2)
;          multiplier   (SP+3)
;
;   Regs used:  
;       ax: product
;       bx: negate flag
;       cx: multiplier
;       dx: multiplicand
;
;   C code:
;       int Multiply(int multiplier, int multiplicand)
;       {
;           int product = 0;
;           int negate = 0;
;   
;           // if either factor is zero return zero
;           if (multiplier | multiplicand)
;           { 
;               // if the multiplier is negative flag it and negate the multiplier
;               if (multiplier < 0)
;               {
;                   negate = !0;
;                   multiplier = -multiplier;
;               }
;               
;               // add the multiplicand to itself multiplier times
;               while (multiplier)
;               {
;                   product += multiplicand;
;                   --multiplier;
;               }
;   
;               // negate the product if necessary
;               if (negate)
;               {
;                   product = -product;
;               }    
;           }
;       
;           return product;
;       }
;

#ifndef MULTIPLY_ASM
#define MULTIPLY_ASM

_Multiply:
    push    bp                  ; setup the stack frame
    mov     bp,sp 
    push    bx                  ; stash the context
    push    cx
    push    dx

    mov     dx,[bp+3]           ; load the multiplicand into dx
    mov     cx,[bp+4]           ; load the multiplier into cx

    zero    ax                  ; if either factor is zero return zero
    or      dx,#0
    bz      _Multiply_Return
    or      cx,#0
    bz      _Multiply_Return

    zero    bx                  ; clear the negate flag
    sub     cx,#0               ; if the multiplier is negative
    bpl     _Multiply_Product
    mov     bx,@0xffff          ; ... set negate flag
    mov     ax,cx               ; ... negate the multiplier
    zero    cx
    sub     cx,ax
    
_Multiply_Product:
    zero    ax                  ; clear the product
_Multiply_L1:    
    add     ax,dx               ; add the multiplicand to the product multiplier times
    dec     cx
    bz      _Multiply_NegateProduct
    bra     _Multiply_L1
    
_Multiply_NegateProduct:
    or      bx,#0               ; negate the product if necessary
    bz      _Multiply_Return
    sub     cx,ax
    mov     ax,cx

_Multiply_Return:
    pop     dx                  ; restore the context
    pop     cx
    pop     bx
    mov     sp,bp               ; return from the subroutine
    pop     bp
    rts
    
#endif // MULTIPLY_ASM

