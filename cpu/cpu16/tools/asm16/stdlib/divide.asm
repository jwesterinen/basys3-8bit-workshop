;   divide.asm
;
;   Subroutine: _Divide
;
;   Description: This function performs a 16-bit division.
;
;   Synopsis: int _Divide(int dividend, int divisor);
;
;   Args:
;       dividend: the number divided by the divisor
;       divisor: the number with which to divide the dividend
;
;   Return: quotient of the dividend and divisor
;
;   Stack on entry:
;     SP-> 
;          retaddr
;          divisor  (SP+2)
;          dividend (SP+3)
;
;   Regs used: ax, bx, cx, dx, ep
;
;   C code:
;       int Divide(int dividend, int divisor)
;       {
;           int quotient = 0;             // init quotient
;           int negDivisor = 0;
;           int negDividend = 0;
;       
;           // if either factor is zero return zero
;           if (dividend | divisor)
;           { 
;               // if the divisor is negative flag it and negate the multiplier
;               if (divisor < 0)
;               {
;                   negDivisor = !0;
;                   divisor = -divisor;
;               }
;               
;               // if the dividend is negative flag it and negate the multiplier
;               if (dividend < 0)
;               {
;                   negDividend = !0;
;                   dividend = -dividend;
;               }
;               
;               // subtract the divisor from the dividend until the divisor is smaller than the dividend
;               // inc the quotient each time the divisor is subtracted from the dividend to form the quotient
;               while (divisor <= dividend)
;               {
;                   dividend -= divisor;
;                   quotient++;
;               }
;                   
;               if ((negDivisor && !negDividend) || (!negDivisor && negDividend))
;               {
;                   quotient = - quotient;
;               }
;           }
;       
;           return quotient;
;       }
;

#ifndef DIVIDE_ASM
#define DIVIDE_ASM

_Divide:
    push    bp                  ; setup the stack frame
    mov     bp,sp
    push    bx                  ; stash the context
    push    cx
    push    dx
    push    ep

    mov     dx,[bp+3]           ; load the multiplicand into dx
    mov     cx,[bp+4]           ; load the multiplier into cx

    zero    ax                  ; if either factor is zero return zero
    or      dx,#0
    bz      _Divide_Return
    or      cx,#0
    bz      _Divide_Return

    zero    bx                  ; clear both negate flags (least significant 2 bits in bx)
    sub     cx,#0               ; if the dividend is negative...
    bpl     _Divide_L1
    or      bx,@0x0001          ; ... set negate dividend flag
    mov     ax,cx               ; ... negate the dividend
    zero    cx
    sub     cx,ax
_Divide_L1:
    sub     dx,#0               ; if the divisor is negative
    bpl     _Divide_CalcQuotient
    or      bx,@0x0002          ; ... set negate divisor flag
    mov     ax,dx               ; ... negate the divisor
    zero    dx
    sub     dx,ax
    
_Divide_CalcQuotient:
    zero    ax                  ; clear the quotient
_Divide_L2:    
    sub     cx,dx               ; subtract the divisor from the dividend
    inc     ax                  ; inc the quotient for each subtraction
    mov     ep,cx               ; continue while cx >= dx
    sub     ep,dx
    bmi     _Divide_CheckNegation ; cx < dx so stop
    bra     _Divide_L2          ; cx >= dx so continue
    
_Divide_CheckNegation:
    mov     cx,@0x0001          ; if bx is 0x0001 or 0x0010 negate the quotient
    sub     cx,bx
    bz      _Divide_NegateQuotient    
    mov     cx,@0x0002
    sub     cx,bx               ; load dx with negDivisor flag
    bz      _Divide_NegateQuotient
    bra     _Divide_Return
    
_Divide_NegateQuotient:
    mov     bx,ax               ; negate ax
    zero    ax
    sub     ax,bx

_Divide_Return:
    pop     ep                  ; restore the context
    pop     dx
    pop     cx
    pop     bx
    mov     sp,bp               ; return from the subroutine
    pop     bp
    rts
    
#endif // DIVIDE_ASM

