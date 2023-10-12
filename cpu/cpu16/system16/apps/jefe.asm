;   hello_world_hw
;
;   This app displays "JEFE" and shows a bouncing LED going from one end of the
;   LEDs to the other.

#include "../stdlib/system16.asm"
#include "../stdlib/sys.asm"
#include "../stdlib/delayms.asm"
#include "../stdlib/display.asm"

Main:
    mov     ax,@0x0efe      ; display "JEFE"
    push    ax
    jsr     Display
    pop     ax
    mov     ax,@18          ; 'J'
    mov     DISPLAY1_REG,ax

    mov     cx,@0xffff      ; init to moving right
    mov     ax,@0x0001      ; init LED value

Loop:    
    mov     LED_REG,ax      ; set the current LED value

Delay:                      ; delay 16mSec
    mov     ax,@16
    push    ax
    jsr     Display
    ;pop     ax
    ;push    ax
    jsr     DelayMs
    pop     ax

CheckEndCases:              ; if (value == 0x8000 || value == 0x0001) change direction
    mov     ax,LED_REG
    mov     bx,ax
    sub     bx,@0x8000
    bz      ChangeDir
    mov     bx,ax
    sub     bx,@0x0001
    bz      ChangeDir
    bra     Shift
    
ChangeDir:    
    xor     cx,@0xffff      ; invert cx to change the shift direction

Shift:                      ; if (!cx) shift left else shift right
    or      cx,@0
    bnz     Right
Left:    
    asl     ax
    bra     Loop    
Right:    
    lsr     ax
    bra     Loop

