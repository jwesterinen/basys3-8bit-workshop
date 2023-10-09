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
;     SP-> retaddr
;          ms      (SP+1)
;
;   Regs used:  ax, bx
;

.define LOOPS_PER_MS 0x2710

DelayMs:
    mov     bx,sp               ; load bx with ms count
    add     bx,#2
    mov     ax,[bx]
    
DelayMs_ms_count_loop:
    or      ax,@0               ; if ms count == 0, exit
    bz      DelayMs_end    
    dec     ax                  ; dec ms count
    
    mov     bx,@LOOPS_PER_MS    ; loop for the number of cycles for one ms
DelayMs_1_ms_loop:
    dec     bx
    bnz     DelayMs_1_ms_loop
    
    bra     DelayMs_ms_count_loop

DelayMs_end:
    rts    
    
