; subroutine test

; SP = 0x0fff;
; while (1)
; {
;     push(GetSwitch());
;     display(pop());
; }

#include "../system16.h"

Begin:
    mov     sp,@0x0fff
    
Loop:
    mov     ax,SWITCH_REG       ; ax = switch[7:4]
    mov     bx,ax               ; bx = switch[3:0]
    and     ax,#0xf0
    lsr     ax
    lsr     ax
    lsr     ax
    lsr     ax
    mov     DISPLAY1_REG,ax
    and     bx,#0x0f
    mov     DISPLAY2_REG,bx

    mov     ex,@Add             ; call Add with params in ax and bx
    jsr     ex 
            
EndLoop:
    mov     DISPLAY4_REG,ax     ; display results
    jmp     Loop

Add:
    add     ax,bx
    rts

