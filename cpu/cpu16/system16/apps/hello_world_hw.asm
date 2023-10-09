;   hello_world_hw
;
;   This is the canonical hello world HW app.  It blinks the LEDs once a second.

#include "../stdlib/system16.asm"
#include "../stdlib/sys.asm"
#include "../stdlib/delayms.asm"

.dz ledVal

Main:
    mov     ax,@0xffff      ; init LED value

Loop:    
    mov     LED_REG,ax      ; set the current LED value
    ;push    ax             ; store ax either on the stack or in ZP memory
    mov     [#ledVal],ax    ; ZP store

    mov     bx,@1000        ; call DelayMs(500)
    push    bx
    jsr     DelayMs
    pop     bx              ; (clean up the stack)
    
DelayMs_toggle_leds:
    ;pop     ax             ; load ax either from the stack or ZP memory
    mov     ax,[#ledVal]    ; ZP load
    xor     ax,@0xffff      ; toggle the LEDs
    bra     Loop
    

