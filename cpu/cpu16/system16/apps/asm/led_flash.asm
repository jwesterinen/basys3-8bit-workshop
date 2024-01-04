;   led_flash.asm
;
;   This app shows a bouncing LED going from one end of the LEDs to
;   the other at a speed chosen with the switches.

#include <system16/system16.asm>
#include <system16/sys.asm>
#include <system16/libasm.asm>

main:
    mov     cx,@0xffff      ; init to moving right
    mov     ax,@0x0001      ; init LED value

Loop:    
    mov     LED_REG,ax      ; set the current LED value

SetSpeed:                   ; set the speed based on the switches
    mov     dx,SWITCH_REG
    or      dx,@0
    bnz     L1
    mov     ax,@500    
    bra     Delay
L1:    
    mov     dx,SWITCH_REG
    sub     dx,@0x0001
    bnz     L2
    mov     ax,@250
    bra     Delay
L2:    
    mov     dx,SWITCH_REG
    sub     dx,@0x0003
    bnz     L3
    mov     ax,@125
    bra     Delay
L3:    
    mov     dx,SWITCH_REG
    sub     dx,@0x0007
    bnz     L4
    mov     ax,@64
    bra     Delay
L4:    
    mov     dx,SWITCH_REG
    sub     dx,@0x000f
    bnz     L5
    mov     ax,@32
    bra     Delay
L5:
    mov     ax,@16

Delay:                      ; delay and display the speed
    push    ax
    jsr     _Display
    ;pop     ax
    ;push    ax
    jsr     _DelayMs
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

