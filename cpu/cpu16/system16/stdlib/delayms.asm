;   delayms.asm
;
;   This file contains the Ms delay subroutine.
;
; void DelayMs(int ms)
;   Delay the given number of mSec.
;   Args:
;     ms: the number of mSec to delay
;   Return: none
;
;   Stack on entry:
;     SP-> 
;      |    retaddr
;      ->   ms      (SP+2)
;
;   Regs used:  ax, bx
;

.define LOOPS_PER_MS 0x2710

DelayMs:
    mov     bp,sp               ; init the stack frame
    
    mov     ax,[bp+2]           ; load ax with the ms argument
    
DelayMs_ms_count_loop:
    or      ax,@0               ; if (ms_count == 0) return
    bz      DelayMs_end    
    dec     ax                  ; dec ms count
    
    mov     bx,@LOOPS_PER_MS    ; loop for the number of cycles for one ms
DelayMs_1_ms_loop:
    dec     bx
    bnz     DelayMs_1_ms_loop
    
    bra     DelayMs_ms_count_loop

DelayMs_end:
    rts    
    
