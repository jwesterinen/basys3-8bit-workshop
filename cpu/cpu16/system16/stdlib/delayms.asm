;   delayms.asm
;
;   Subroutine: _DelayMs
;
;   Description: This subroutine performs a blocking delay.  It delays the 
;   number of mSec in AX.
;
;   Synopsis: void _DelayMs(int ms)
;
;   Args:
;     ax: the number of mSec to delay
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

.define LOOPS_PER_MS 0x2710

_DelayMs:
    mov     bp,sp               ; setup the stack frame
    push    bx
    
    mov     ax,[bp+2]           ; load ax with the ms argument    
    
_DelayMs_ms_count_loop:
    or      ax,@0               ; mS count loop
    bz      _DelayMs_end    
    dec     ax
    
    mov     bx,@LOOPS_PER_MS    ; 1mS delay loop
_DelayMs_1_ms_loop:
    dec     bx
    bnz     _DelayMs_1_ms_loop
    
    bra     _DelayMs_ms_count_loop

_DelayMs_end:
    pop     bx
    rts    

#endif // DELAY_ASM       
