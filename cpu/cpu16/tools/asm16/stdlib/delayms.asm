;   delayms.asm
;
;   Subroutine: _DelayMs
;
;   Description: millisecond delay
;
;   Synopsis: void _DelayMs(int ms)
;
;   Args:
;       ms: the number of mSec to delay
;
;   Return: none
;
;   Stack on entry:
;     SP-> 
;      |    retaddr
;      ->   ms      (SP+2)
;

#ifndef DELAY_ASM
#define DELAY_ASM

#define MS [bp+3]
  
.define LOOPS_PER_MS 0x2710

_DelayMs:
    push    bp
    mov     bp,sp               ; setup the stack frame
    push    bx
    
    mov     ax,MS               ; mS count loop
_DelayMs_msCountLoop:
    or      ax,@0
    bz      _DelayMs_end    
    dec     ax
    
    mov     bx,@LOOPS_PER_MS    ; 1mS delay loop
_DelayMs_1msDelayLoop:
    dec     bx
    bnz     _DelayMs_1msDelayLoop    
    bra     _DelayMs_msCountLoop

_DelayMs_end:
    pop     bx                  ; restore stack and return
    mov     sp,bp
    pop     bp
    rts    

#endif // DELAY_ASM       
