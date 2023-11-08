;   jefe.asm
;
;   This app shows a bouncing LED going from one end of the LEDs to
;   the other at a speed chosen with the switches.

#include <asm16/system16.asm>
#include <asm16/sys.asm>
#include <asm16/libasm.asm>

Main:
    mov     ax,@0x0efe      ; display "JEFE"
    push    ax
    jsr     _Display
    pop     ax
    mov     ax,@0x0012
    mov     DISPLAY1_REG,ax ; ('J')
    
    mov     cx,@0xffff      ; init to moving right
    mov     ax,@0x0001      ; init LED value

Loop:    
    mov     LED_REG,ax      ; set the current LED value

Delay:
    mov     ax,@16          ; delay 16 mSec
    push    ax
    jsr     _DelayMs
    pop     ax

CheckEndCases:
    mov     ax,LED_REG      ; if (value == 0x8000 || value == 0x0001) change direction
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

