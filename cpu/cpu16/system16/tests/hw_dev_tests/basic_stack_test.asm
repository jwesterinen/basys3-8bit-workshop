; stack test

; SP = 0x0fff;
; while (1)
; {
;     push(GetSwitch());
;     display(pop());
; }

#include <asm16/system16.asm>

Begin:
    mov     sp,@0x0fff
    
Loop:
    jsr     DisplaySwitches
    bra     Loop
    
End:

DisplaySwitches:
    mov     ax,SWITCH_REG
    push    ax
    zero    ax
    push    ax
    inc     ax
    push    ax
    pop     bx
    pop     bx
    pop     bx
    mov     LED_REG,bx
    rts

