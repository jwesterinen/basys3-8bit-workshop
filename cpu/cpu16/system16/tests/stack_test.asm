; stack test

; SP = 0x0fff;
; while (1)
; {
;     push(GetSwitch());
;     display(pop());
; }

#include "../system16.h"

.org 0xf000

Begin:
    mov     sp,@0x0fff
    
Loop:
    mov     ax,SWITCH_REG
    push    ax
    pop     bx
    mov     LED_REG,bx
    
End:
    bra     Loop

