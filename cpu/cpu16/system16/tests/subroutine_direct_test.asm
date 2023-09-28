; subroutine test

; SP = 0x0fff;
; while (1)
; {
;     op1 = GetSwitches() & 0x00f0 >> 4;
;     display(op1, 1);
;     op2 = GetSwitches() & 0x000f;
;     display(op2, 2);
;
;     if (GetSwitches() & 0x8000)
;         val = Add(op1, op2);
;     else
;         val = Sub(op1, op2);
;     display(val, 4);
; }

#include "../system16.h"

Begin:
    mov     sp,@0x0fff
    bra     Loop
    
Add:
    add     ax,bx
    rts
    
Loop:
    mov     ax,SWITCH_REG       ; ax = switch[7:4]
    mov     bx,ax               ; bx = switch[3:0]
    mov     cx,ax               ; cx = switch[15]
    and     ax,#0xf0
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     DISPLAY1_REG,ax
    and     bx,#0x0f
    mov     DISPLAY2_REG,bx
    and     cx,@0x8000
    
    sub     cx,#0
    bz      L1
    bras    Sub                 ; if switch != 0, call Sub via branch with params in ax and bx
    bra     L2
L1:
    bras    Add                 ; if switch == 0, call Add via branch with params in ax and bx
L2:
            
EndLoop:
    mov     DISPLAY4_REG,ax   ; display results
    jmp     Loop
    
Sub:
    sub     ax,bx
    rts
    

